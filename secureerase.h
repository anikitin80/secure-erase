#pragma once

#include "QIcon.h"

class CEraseMethodBase;

class FileRemoveInfo
{
public:

    enum OpStatus
    {
        NotStarted = 0,
        Started,
        Finished
    };

    FileRemoveInfo(const QString& path, bool bDir)
        : Path(path), IsDirectory(bDir)
    {
    };

    bool operator<(const FileRemoveInfo& other) const
    {
        return Path < other.Path;
    }

    QString Path;
    bool IsDirectory = false;
    qint64 Size = 0;
    QIcon Icon;
    OpStatus Status = NotStarted;
    int LastError = 0;
};

int SecureEraseFile(QString filePath, CEraseMethodBase* pMethod, bool& bCancel);
