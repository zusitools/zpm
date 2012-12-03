#ifndef PACKAGE_H
#define PACKAGE_H

#include <QList>
#include <QVariant>
#include "packageversion.h"

/**
 * @brief The possible installation states a package can have.
 */
enum PackageState {
    AUTODELETE,
    AUTOINSTALL,
    AUTOUPDATE,
    DELETE,
    INSTALL,
    KEEPINSTALLED,
    NOTINSTALLED,
    PROTECTED,
    TABOO,
    UPDATE
};

/**
 * @brief Represents a package contained in a folder. A package can contain versions.
 */
class Package
{
public:
    Package(QString qualifiedName, QString displayName = QString(), PackageState state = NOTINSTALLED);
    ~Package();

    QString getQualifiedName();
    QString getDisplayName();
    PackageState state() { return m_state; }
    void setState(PackageState state) { m_state = state; }

private:
    QString qualifiedName;
    QString displayName;
    PackageState m_state;
    QList<PackageVersion *> *versions;
    PackageVersion *installedVersion;
};

#endif // PACKAGE_H
