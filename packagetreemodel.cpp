#include "packagetreemodel.h"
#include "package.h"
#include "folder.h"
#include <QStringList>
#include <QDebug>
#include <QIcon>

PackageTreeModel::PackageTreeModel(QList<Package *> *packages, QObject *parent)
    : QAbstractItemModel(parent)
{
    this->rootFolder = new Folder();

    foreach(Package * package, *packages) {
        insertPackage(package);
    }
}

PackageTreeModel::~PackageTreeModel()
{
    delete rootFolder;
}

int PackageTreeModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return this->rootFolder->subfolderCount() + this->rootFolder->packageCount();
    } else {
        PackageTreeItem *item = static_cast<PackageTreeItem *>(parent.internalPointer());

        if (item->getType() == FOLDER) {
            return ((Folder *)item)->subfolderCount() + ((Folder *)item)->packageCount();
        }

        return 0;
    }
}

int PackageTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

QVariant PackageTreeModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        PackageTreeItem *item = static_cast<PackageTreeItem*>(index.internalPointer());

        if (role == Qt::DisplayRole) {
            if (item->getType() == FOLDER && index.column() == 0) {
                return QString(((Folder*)item)->getName());
            } else if (item->getType() == PACKAGE) {
                switch (index.column()) {
                    case 0: return ((PackageItem*)item)->package()->getDisplayName();
                    case 1: return QString("1.0.1");
                    case 2: return QString("1.0.2");
                    case 3: return QString("Max Mustermann");
                }
            }
        }
        else if (role == Qt::CheckStateRole && item->getType() == PACKAGE && index.column() == 0)
        {
            // We have to return Qt::Checked or Qt::Unchecked, else check boxes won't be shown.
            // The value returned is irrelevant since we do the drawing of the check boxes ourselves
            // based on the package state.
            return Qt::Checked;
        }
        else if (role == Qt::DecorationRole && index.column() == 0)
        {
            if (item->getType() == FOLDER) {
                return QIcon("icons/folder.png"); // TODO os specific icons
            } else if (item->getType() == PACKAGE) {
                return QIcon("icons/package.png"); // TODO file specific icons
            }
        }
    }

    return QVariant();
}

QModelIndex PackageTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    Folder *parentFolder = indexToFolder(parent);

    if (row < 0 || row >= parentFolder->packageCount() + parentFolder->subfolderCount()) {
        return QModelIndex();
    } else if (row < parentFolder->packageCount()) {
        return createIndex(row, column, parentFolder->package(row));
    } else {
        return createIndex(row, column, parentFolder->subfolder(row - parentFolder->packageCount()));
    }
}

QModelIndex PackageTreeModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) {
        return QModelIndex();
    }

    PackageTreeItem *item = static_cast<PackageTreeItem*>(child.internalPointer());

    if (item == NULL || item == rootFolder) {
        return QModelIndex();
    }

    Folder *parent = item->parentFolder();

    for (int i = 0; i < parent->packageCount(); i++) {
        if (parent->package(i) == item) {
            return createIndex(i, 0, parent);
        }
    }

    for (int i = 0; i < parent->subfolderCount(); i++) {
        if (parent->subfolder(i) == item) {
            return createIndex(i + parent->packageCount(), 0, parent);
        }
    }

    return QModelIndex();
}

Qt::ItemFlags PackageTreeModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags result = 0;

    if (index.isValid()) {
        result = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

        PackageTreeItem *item = static_cast<PackageTreeItem*>(index.internalPointer());
        if (item->getType() == PACKAGE) {
            result |= Qt::ItemIsUserCheckable;
        }
    }

    return result;
}

void PackageTreeModel::notifyDataChanged(const QModelIndex &index)
{
    emit dataChanged(index, index);
}

QVariant PackageTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return tr("Name");
            case 1:
                return tr("Installed Version");
            case 2:
                return tr("Available Version");
            case 3:
                return tr("Author");
        }
    }

    return QVariant();
}

bool PackageTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::CheckStateRole && ((PackageTreeItem*)(index.internalPointer()))->getType() == PACKAGE) {
        emit packageCheckStateChanged(index);
        emit dataChanged(index, index);
        return true;
    }

    return QAbstractItemModel::setData(index, value, role);
}

Folder *PackageTreeModel::indexToFolder(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return this->rootFolder;
    } else {
        return static_cast<Folder*>(index.internalPointer());
    }
}

void PackageTreeModel::insertPackage(Package *package) {
    QStringList splitPath = package->getQualifiedName().split("/");
    Folder *parent = this->rootFolder;

    for (int i = 0; i < splitPath.count() - 1; i++) {
        bool foundParent = false;

        for (int j = 0; j < parent->subfolderCount(); j++) {
            if (parent->subfolder(j)->getName() == splitPath.at(i)) {
                foundParent = true;
                parent = parent->subfolder(j);
                break;
            }
        }

        if (!foundParent) {
            Folder *newSubfolder = new Folder(splitPath.at(i), parent);
            parent->appendSubfolder(newSubfolder);
            parent = newSubfolder;
        }
    }

    // Check if a package with the same name already exists in the folder
    for (int i = 0; i < parent->packageCount(); i++) {
        if (parent->package(i)->package()->getQualifiedName().toLower() == package->getQualifiedName().toLower()) {
            // TODO: we would append the versions here
            return;
        }
    }

    parent->appendPackage(new PackageItem(package, parent));
}
