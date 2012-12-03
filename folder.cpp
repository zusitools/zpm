#include <QStringList>
#include "folder.h"
#include "package.h"

Folder::Folder(const QString &name, Folder *parent) : PackageTreeItem(parent)
{
    this->name = name;
}

Folder::~Folder()
{
    qDeleteAll(subfolders);
    qDeleteAll(packages);

    subfolders.clear();
    packages.clear();
}

void Folder::appendSubfolder(Folder *child)
{
    subfolders.append(child);
}

void Folder::appendPackage(PackageItem *package)
{
    packages.append(package);
}

Folder *Folder::subfolder(int row)
{
    return subfolders.at(row);
}

PackageItem *Folder::package(int row)
{
    return packages.at(row);
}

int Folder::subfolderCount() const
{
    return subfolders.count();
}

int Folder::packageCount() const
{
    return packages.count();
}

QString Folder::getName()
{
    return name;
}
