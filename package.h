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
    PackageState originalState() { return m_originalState; }
    void setState(PackageState state) { m_state = state; }

    void appendVersion(PackageVersion *version) { m_versions->append(version); }
    void setInstalledVersion(PackageVersion *version) { m_installedVersion = version; }

    PackageVersion *installedVersion() { return m_installedVersion; }

    void addDependency(PackageFile *file) { m_dependencies.append(file); }
    QList<PackageFile *> dependencies() { return m_dependencies; }

private:
    QString qualifiedName;
    QString displayName;
    PackageState m_state;
    const PackageState m_originalState;
    QList<PackageVersion *> *m_versions;
    QList<PackageFile *> m_dependencies;
    PackageVersion *m_installedVersion;
};

#endif // PACKAGE_H
