#ifndef FOLDER_H
#define FOLDER_H

#include <QList>
#include <QVariant>
#include "packagetreeitem.h"
#include "packageitem.h"

// Forward declaration of class PackageItem, Folder and PackageItem reference each other.
class PackageItem;

class Folder : public PackageTreeItem
{
public:
    Folder(const QString &name = "", Folder *parent = 0);
    virtual ~Folder();

    void appendSubfolder(Folder *child);
    void appendPackage(PackageItem *package);

    Folder *subfolder(int row);
    PackageItem *package(int index);

    int subfolderCount() const;
    int packageCount() const;

    int totalSubItemsCount() const;

    QString getName();

    PackageTreeItemType getType() { return FOLDER; }

private:
    QList<Folder *> subfolders;
    QList<PackageItem *> packages;
    QString name;
};

#endif // FOLDER_H
