#include "packagetreesortfilterproxymodel.h"
#include "packagetreeitem.h"
#include "package.h"
#include "folder.h"

PackageTreeSortFilterProxyModel::PackageTreeSortFilterProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
}

bool PackageTreeSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    PackageTreeItem *leftItem = (PackageTreeItem*)left.internalPointer();
    PackageTreeItem *rightItem = (PackageTreeItem*)right.internalPointer();

    if (leftItem->getType() == FOLDER) {
        return (rightItem->getType() != PACKAGE) && (((Folder*)leftItem)->getName() < ((Folder*)rightItem)->getName());
    } else { // left->getType() == PACKAGE
        return (rightItem->getType() != FOLDER) || (((Package*)leftItem)->getDisplayName() < ((Package*)rightItem)->getDisplayName());
    }
}
