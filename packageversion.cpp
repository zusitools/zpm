#include "packageversion.h"

PackageVersion::PackageVersion(QList<PackageFile *> files, Package *package, QString repo, bool installed)
{
    m_files = files;
    m_package = package;
    m_repo = repo;

    m_installed = installed;
    m_keepInstalled = installed;
}
