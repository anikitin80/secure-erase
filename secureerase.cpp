#include "secureerase.h"
#include "QApplication"
#include "QFile"
#include "erasemethod.h"

#ifdef _WIN32
    #include "windows.h"
#elif __linux__
    #include <fcntl.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <sys/param.h>
    #include <stdio.h>
#endif

#ifdef _WIN32

// Windows version - use low-level WinAPI functions for writing direct to file without caching

int SecureEraseFile(QString filePath, CEraseMethodBase* pMethod, bool& bCancel)
{
    bool bResult = false;
    int  errorCode = 0;
    HANDLE hFile = INVALID_HANDLE_VALUE;

    // convert QString to wchar array
    wchar_t path[_MAX_PATH] = {0};
    int iLen = filePath.toWCharArray(path);
    path[iLen] = '\0';

    DWORD dwFlagsAndAttributes = FILE_FLAG_WRITE_THROUGH | FILE_FLAG_SEQUENTIAL_SCAN;
    DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
    hFile = ::CreateFile(path, FILE_WRITE_DATA, dwShareMode, 0, OPEN_EXISTING, dwFlagsAndAttributes, 0);

    if (INVALID_HANDLE_VALUE != hFile)
    {
        bResult = ::FlushFileBuffers(hFile);

        if (bResult)
        {
            INT64 allocationSize = 0;

            bResult = GetFileSizeEx(hFile, (PLARGE_INTEGER) &allocationSize);

            if (bResult)
            {
                pMethod->ResetIteration();
                while (bResult && pMethod->NextIteration())
                {
                    LARGE_INTEGER to {{0, 0}};
                    bResult = SetFilePointerEx(hFile, to, NULL, FILE_BEGIN);

                    INT64 done = 0;
                    while (done < allocationSize)
                    {
                        if (bCancel)
                        {
                            ::CloseHandle(hFile);
                            return 0;
                        }

                        DWORD written = 0;
                        DWORD nNeedToWrite = (DWORD) qMin(pMethod->GetBlockSize(), allocationSize - done);

                        bResult = WriteFile(hFile, pMethod->GetOverwriteBlock().data(), nNeedToWrite, &written, NULL);
                        if(!bResult)
                            break;

                        qApp->processEvents();  // need to avoid UI freezing
                        done += written;                        
                    }
                }
            }
        }

        if (bResult)
        {
            HANDLE hDeleteFile = ::CreateFile(path, FILE_GENERIC_READ, dwShareMode, 0, OPEN_EXISTING, FILE_FLAG_DELETE_ON_CLOSE, 0);

            if (INVALID_HANDLE_VALUE != hDeleteFile)
            {
                ::CloseHandle(hFile);
                ::CloseHandle(hDeleteFile);

                hFile = INVALID_HANDLE_VALUE;
            }
            else
            {
                bResult = false;
            }
        }

        if (INVALID_HANDLE_VALUE != hFile)
        {
            ::CloseHandle(hFile);
        }
    }

    if(!bResult)
        errorCode = GetLastError();
    else
        errorCode = 0;

    return errorCode;
}

#elif __linux__

// Linux version of secure erase

int SecureEraseFile(QString filePath, CEraseMethodBase* pMethod, bool& bCancel)
{
    // open file
    int fd = open(filePath.toUtf8(), O_SYNC|O_RDWR);
    if(fd == -1)
        return errno;

    // get file size
    struct stat64 file_stat;
    fstat64(fd, &file_stat);
    int64_t allocation_size = file_stat.st_size;

    // pass through all method's iterations
    pMethod->ResetIteration();
    while (pMethod->NextIteration())
    {
        lseek(fd, 0, SEEK_SET);

        int64_t done = 0;
        while (done < allocation_size)
        {
            if (bCancel)
            {
                close(fd);
                return 0;
            }

            int64_t nNeedToWrite = MIN(pMethod->GetBlockSize(), allocation_size - done);
            int64_t written = write(fd, (void*)pMethod->GetOverwriteBlock().data(), nNeedToWrite);

            if(-1 == written)
            {
                close(fd);
                return errno;
            }

            qApp->processEvents();  // need to avoid UI freezing
            done += written;
        }

        fsync(fd);
    }

    //close file
    close(fd);

    // remove file
    if(-1 == remove(filePath.toUtf8()))
        return errno;

    return 0;
}

#endif
