#include "package.h"
#include <QList>
#include <QStringList>

Package::Package(QString qualifiedName)
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
