#ifndef PACKAGE_H
#define PACKAGE_H

#include <QList>
#include <QVariant>
#include "packagetreeitem.h"
#include "packageversion.h"
#include "folder.h"

// Forward declaration of class Folder, Folder and Package reference each other.
class Folder;

/**
 * @brief Represents a package contained in a folder. A package can contain versions.
 */
class Package : public PackageTreeItem
{
public:
    Package(QString qualifiedName, Folder *parent = 0);
    ~Package();

    QString getQualifiedName();
    QString getDisplayName();

    PackageTreeItemType getType() { return PACKAGE; }

private:
    QString qualifiedName;
    QString displayName;
    QList<PackageVersion *> *versions;
    PackageVersion *installedVersion;
};

#endif // PACKAGE_H
