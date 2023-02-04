#pragma once

#include "secureerase.h"
#include <QAbstractTableModel>

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
    void GetFilesToRemove(QList<FileRemoveInfo>& files);
    int GetItemsCount();
    FileRemoveInfo& GetItem(int nIndex);
    void UpdateStatus(int nRow, FileRemoveInfo::OpStatus status);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QList<FileRemoveInfo> FilesList;
    QIcon ImgError;
    QIcon ImgSuccess;
    QIcon ImgErase;

};

