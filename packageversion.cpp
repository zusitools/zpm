#include "packageversion.h"

PackageVersion::PackageVersion(QList<PackageFile *> files, Package *package, QString repo)
{
    m_files = files;
    m_package = package;
    m_repo = repo;
}
