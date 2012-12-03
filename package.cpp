#include "package.h"
#include <QList>
#include <QStringList>

Package::Package(QString qualifiedName, Folder *parent) : PackageTreeItem(parent)
{
    this->qualifiedName = qualifiedName;
    this->displayName = qualifiedName.split("/").last();
    this->versions = new QList<PackageVersion *>();
}

Package::~Package()
{
    // qDeleteAll(versions);
}

QString Package::getQualifiedName()
{
    return qualifiedName;
}

QString Package::getDisplayName()
{
    return displayName;
}
