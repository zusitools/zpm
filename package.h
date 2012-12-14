#ifndef PACKAGE_H
#define PACKAGE_H

#include <QList>
#include <QVariant>
#include "packageversion.h"
#include "packagefile.h"

class PackageVersion;
class PackageFile;

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
    Package(QString qualifiedName, QString displayName = QString());
    ~Package();

    QString getQualifiedName();
    QString getDisplayName();

    /**
     * Returns the current state of the package.
     */
    PackageState state();

    /**
     * Returns the state of the package when it was read in from the repository file.
     */
    PackageState originalState();

    QList<PackageVersion *> *versions() { return m_versions; }
    void appendVersion(PackageVersion *version) { m_versions->append(version); }

    PackageVersion *installedVersion();
    PackageVersion *versionToInstall();

    void setAutoChange(bool autoChange) { m_autoChange = autoChange; }

private:
    QString qualifiedName;
    QString displayName;

    QList<PackageVersion *> *m_versions;

    bool m_autoChange;
    bool m_protected;
};

#endif // PACKAGE_H
