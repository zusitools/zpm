#ifndef PACKAGEVERSION_H
#define PACKAGEVERSION_H

#include "packagefile.h"
#include "package.h"
#include <QList>

class Package;
class PackageFile;

class PackageVersion
{
public:
    PackageVersion(QList<PackageFile *> files, Package *package, QString repo, bool installed);

    QList<PackageFile *> *files() { return &m_files; }
    Package *package() { return m_package; }
    QString repo() { return m_repo; }

    void addDependency(PackageFile *file) { m_dependencies.append(file); }
    QList<PackageFile *> dependencies() { return m_dependencies; }

    bool installed() { return m_installed; }
    bool keepInstalled() { return m_keepInstalled; }
    void setKeepInstalled(bool keepInstalled) { m_keepInstalled = keepInstalled; }
private:
    QList<PackageFile *> m_files;
    QList<PackageFile *> m_dependencies;
    Package *m_package;
    QString m_repo; // TODO: own class for repo with links to packages?

    bool m_installed;
    bool m_keepInstalled;
};

#endif // PACKAGEVERSION_H
