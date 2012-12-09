#include "package.h"
#include <QList>
#include <QStringList>

Package::Package(QString qualifiedName, QString displayName)
{
    this->qualifiedName = qualifiedName;

    if (displayName.isEmpty()) {
        this->displayName = qualifiedName.split("/").last();
    } else {
        this->displayName = displayName;
    }

    this->m_versions = new QList<PackageVersion *>();
    this->m_protected = false; // TODO
    this->m_autoChange = false; // TODO
}

Package::~Package()
{
    qDeleteAll(*m_versions);
    delete m_versions;
}

QString Package::getQualifiedName()
{
    return qualifiedName;
}

QString Package::getDisplayName()
{
    return displayName;
}

PackageState Package::state()
{
    PackageVersion *l_installedVersion = installedVersion();
    PackageVersion *l_versionToInstall = versionToInstall();

    if (l_installedVersion == l_versionToInstall) {
        return originalState();
    } else {
        if (l_versionToInstall == NULL) {
            return (m_autoChange ? AUTODELETE : DELETE);
        } else {
            if (l_installedVersion == NULL) {
                return (m_autoChange ? AUTOINSTALL : INSTALL);
            } else {
                return (m_autoChange ? AUTOUPDATE : UPDATE);
            }
        }
    }
}

PackageState Package::originalState()
{
    if (installedVersion() == NULL) {
        return (m_protected ? TABOO : NOTINSTALLED);
    } else {
        return (m_protected ? PROTECTED : KEEPINSTALLED);
    }
}

PackageVersion *Package::installedVersion()
{
    foreach (PackageVersion* version, *m_versions) {
        if (version->installed()) {
            return version;
        }
    }

    return NULL;
}

PackageVersion *Package::versionToInstall()
{
    foreach (PackageVersion* version, *m_versions) {
        if (version->keepInstalled()) {
            return version;
        }
    }

    return NULL;
}
