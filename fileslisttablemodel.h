#pragma once

#include "qicon.h"
#include <QAbstractTableModel>

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
    int Progress = 0;

};

class FilesListTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:    
    explicit FilesListTableModel(QObject *parent = nullptr);

    enum Columns
    {
        ColPath = 0,
        ColSize,
        ColStatus,
        // add new columns before this line
        ColCount,
        ColLast = ColCount -1
    };

    void AddItem(const QString& path, bool bDir);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QList<FileRemoveInfo> FilesList;
};

