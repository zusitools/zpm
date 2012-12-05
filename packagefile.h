#ifndef PACKAGEVERSIONFILE_H
#define PACKAGEVERSIONFILE_H

#include "packageversion.h"
#include <QList>
#include <QString>

class PackageVersion;

class PackageFile
{
public:
    PackageFile(QString name);

    void addProvider(PackageVersion *provider) { m_providers.append(provider); }
    QList<PackageVersion *> *providers() { return &m_providers; }

    QString name() { return m_name; }
private:
    QList<PackageVersion *> m_providers;
    QString m_name;
};

#endif // PACKAGEVERSIONFILE_H
