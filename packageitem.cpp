#include "packageitem.h"

PackageItem::PackageItem(Package *package, Folder *parent) : PackageTreeItem(parent)
{
    this->m_package = package;
}

Package *PackageItem::package()
{
    return m_package;
}
