#include "packagetreeitem.h"


PackageTreeItem::PackageTreeItem(Folder *parentFolder)
{
    this->m_parentFolder = parentFolder;
}

Folder *PackageTreeItem::parentFolder()
{
    return m_parentFolder;
}
