#include "packagetreeitem.h"


PackageTreeItem::PackageTreeItem(Folder *parentFolder)
{
    this->parentFolder = parentFolder;
}

Folder *PackageTreeItem::getParentFolder()
{
    return parentFolder;
}

void PackageTreeItem::setParentFolder(Folder *parent)
{
    this->parentFolder = parent;
}
