#include "fileslisttablemodel.h"
#include "QFileIconProvider.h"
#include "QDirIterator.h"

FilesListTableModel::FilesListTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

QVariant FilesListTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    switch(section)
    {
    case ColPath:
        return QVariant("Path");
    case ColSize:
        return QVariant("Size");
    case ColStatus:
        return QVariant("Status");
    }

    return QVariant();
}

int FilesListTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return FilesList.size();
}

int FilesListTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return ColCount;
}

QVariant FilesListTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(index.row() < 0 || index.row() >= FilesList.size())
        return QVariant();

    const FileRemoveInfo& item = FilesList.at(index.row());

    switch(index.column())
    {
    case ColPath:
        if(role == Qt::DisplayRole)
            return item.Path;
        if(role == Qt::DecorationRole)
            return item.Icon;
        break;
    case ColSize:
        if(role == Qt::DisplayRole)
            return QVariant(QLocale("en_US").formattedDataSize(item.Size));
        break;
    }

    return QVariant();
}

void FilesListTableModel::AddItem(const QString& path, bool bDir)
{
    FileRemoveInfo item(path, bDir);

    QFileIconProvider iconprovider;
    if(bDir)
    {
        // set folder icon
        item.Icon = iconprovider.icon(QFileIconProvider::Folder);

        // calculate folder size
        QDirIterator it(path, QDirIterator::Subdirectories);
        item.Size = 0;
        while (it.hasNext())
        {
            QFile file(it.next());
            item.Size += file.size();
        }
    }
    else
    {
        // set file icon and size
        QFileInfo fileInfo(path);
        item.Icon = iconprovider.icon(fileInfo);
        item.Size = fileInfo.size();
    }

    // insert new item or update existing item
    auto iter = std::lower_bound(FilesList.begin(), FilesList.end(), item);
    if(iter != FilesList.end() && iter->Path == path)
    {
        int nRow = iter - FilesList.begin();
        *iter = item;
        emit dataChanged(QAbstractItemModel::createIndex(nRow, 0), QAbstractItemModel::createIndex(nRow, ColLast));
    }
    else
    {
        int nRow = iter - FilesList.begin();
        beginInsertRows(QModelIndex(), nRow, nRow);
        FilesList.insert(iter, item);
        endInsertRows();
    }
}

bool FilesListTableModel::removeRows(int row, int count, const QModelIndex& parent)
{
    beginRemoveRows(parent, row, row+count-1);
    for(int nRow = row + count -1; nRow >= row; nRow--)
        FilesList.removeAt(nRow);
    endRemoveRows();
}
