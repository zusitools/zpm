#include "package.h"
#include <QList>
#include <QStringList>

Package::Package(QString qualifiedName, QString displayName = QString())
{
    this->qualifiedName = qualifiedName;

    if (displayName.isEmpty()) {
        this->displayName = qualifiedName.split("/").last();
    } else {
        this->displayName = displayName;
    }

    this->versions = new QList<PackageVersion *>();
}

Package::~Package()
{
    qDeleteAll(*versions);
}

QString Package::getQualifiedName()
{
    return qualifiedName;
}

QString Package::getDisplayName()
{
    return displayName;
}
