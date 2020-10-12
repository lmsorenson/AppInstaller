#include "releaseitem.h"

ReleaseItem::ReleaseItem(QObject *parent)
    : QAbstractListModel(parent)
{
}

QVariant ReleaseItem::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

QModelIndex ReleaseItem::index(int row, int column, const QModelIndex &parent) const
{
    // FIXME: Implement me!
}

QModelIndex ReleaseItem::parent(const QModelIndex &index) const
{
    // FIXME: Implement me!
}

int ReleaseItem::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

int ReleaseItem::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

QVariant ReleaseItem::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}
