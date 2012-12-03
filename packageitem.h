#ifndef PACKAGEITEM_H
#define PACKAGEITEM_H

#include "packagetreeitem.h"
#include "package.h"

// Forward declaration of class Folder, Folder and PackageItem reference each other.
class Folder;

/**
 * @brief A model item that contains a package.
 */
class PackageItem : public PackageTreeItem
{
public:
    PackageItem(Package *package, Folder *parent = 0);
    Package *package();

    PackageTreeItemType getType() { return PACKAGE; }
private:
    Package *m_package;
};

#endif // PACKAGEITEM_H
