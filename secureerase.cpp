#include "secureerase.h"
#include "QApplication"
#include "QFile"
#include "erasemethod.h"

#ifdef _WIN32
    #include "windows.h"
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
                            return true;
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

// linux version - just stub for testing

int SecureEraseFile(QString filePath, CEraseMethodBase* pMethod, bool& bCancel)
{
    if(!QFile::remove(filePath))
        return -1;

    return 0;
}

#endif
