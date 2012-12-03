#ifndef PACKAGE_H
#define PACKAGE_H

#include <QList>
#include <QVariant>
#include "packageversion.h"

/**
 * @brief Represents a package contained in a folder. A package can contain versions.
 */
class Package
{
public:
    Package(QString qualifiedName, QString displayName);
    ~Package();

    QString getQualifiedName();
    QString getDisplayName();

private:
    QString qualifiedName;
    QString displayName;
    QList<PackageVersion *> *versions;
    PackageVersion *installedVersion;
};

#endif // PACKAGE_H
