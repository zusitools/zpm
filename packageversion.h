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
    PackageVersion(QList<PackageFile *> files, Package *package, QString repo);

    QList<PackageFile *> *files() { return &m_files; }
    Package *package() { return m_package; }
    QString repo() { return m_repo; }
private:
    QList<PackageFile *> m_files;
    Package *m_package;
    QString m_repo; // TODO: own class for repo with links to packages?
};

#endif // PACKAGEVERSION_H
