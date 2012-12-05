#ifndef PACKAGETREEMODEL_H
#define PACKAGETREEMODEL_H

#include <QAbstractItemModel>
#include "package.h"
#include "folder.h"

class PackageTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit PackageTreeModel(QList<Package *> *packages, QObject *parent = 0);
    ~PackageTreeModel();

    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual bool hasChildren(const QModelIndex &parent) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    void notifyDataChanged(const QModelIndex &index);


private:
    inline Folder *indexToFolder(const QModelIndex &index) const;
    QModelIndex itemToIndex(PackageTreeItem *item) const;
    void insertPackage(Package *package);
    Folder *rootFolder;
    
signals:
    void packageCheckStateChanged(QModelIndex index) const;
    
public slots:
    
};

#endif // PACKAGETREEMODEL_H
