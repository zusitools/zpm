#ifndef PACKAGEVERSION_H
#define PACKAGEVERSION_H

#include "packagefile.h"
#include <QList>

class PackageFile;

class PackageVersion
{
public:
    PackageVersion(QList<PackageFile *> files);
    QList<PackageFile *> *files() { return &m_files; }
private:
    QList<PackageFile *> m_files;
};

#endif // PACKAGEVERSION_H
