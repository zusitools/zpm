#include "package.h"
#include <QList>
#include <QStringList>

Package::Package(QString qualifiedName, QString displayName, PackageState state)
{
    this->qualifiedName = qualifiedName;

    if (displayName.isEmpty()) {
        this->displayName = qualifiedName.split("/").last();
    } else {
        this->displayName = displayName;
    }

    this->versions = new QList<PackageVersion *>();
    this->m_state = state;
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
