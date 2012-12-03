#ifndef FOLDER_H
#define FOLDER_H

#include <QList>
#include <QVariant>
#include "packagetreeitem.h"
#include "package.h"

// Forward declaration of class Package, Folder and Package reference each other.
class Package;

class Folder : public PackageTreeItem
{
public:
    Folder(const QString &name = "", Folder *parent = 0);
    ~Folder();

    void appendSubfolder(Folder *child);
    void appendPackage(Package *package);

    Folder *subfolder(int row);
    Package *package(int index);

    int subfolderCount() const;
    int packageCount() const;

    int totalSubItemsCount() const;

    QString getName();

    PackageTreeItemType getType() { return FOLDER; }

private:
    QList<Folder *> subfolders;
    QList<Package *> packages;
    QString name;
};

#endif // FOLDER_H
