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
        return (rightItem->getType() != PACKAGE) && (((Folder*)leftItem)->getName().localeAwareCompare(((Folder*)rightItem)->getName()) < 0);
    } else { // left->getType() == PACKAGE
        return (rightItem->getType() != FOLDER) && (((PackageItem*)leftItem)->package()->getDisplayName().localeAwareCompare(((PackageItem*)rightItem)->package()->getDisplayName()) < 0);
    }
}

bool PackageTreeSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex itemIndex = sourceModel()->index(sourceRow, 0, sourceParent);

    // TODO:
    // A folder is visible if one of its descendants is visible or if its name contains the filterRegExp()
    // A package is visible if one of its containing folders is visible or if its name contains the filterRegExp()

    if (sourceModel()->data(itemIndex, Qt::DisplayRole).toString().contains(filterRegExp())) {
        return true;
    } else if (sourceModel()->hasChildren(itemIndex)) {
        for (int i = 0; i < sourceModel()->rowCount(itemIndex); i++) {
            if (filterAcceptsRow(i, itemIndex)) {
                return true;
            }
        }
    }

    return false;
}
