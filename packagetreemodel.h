#ifndef PACKAGETREEMODEL_H
#define PACKAGETREEMODEL_H

#include <QAbstractItemModel>
#include "package.h"

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

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;


private:
    inline Folder *indexToFolder(const QModelIndex &index) const;
    void insertPackage(Package *package);
    Folder *rootFolder;
    
signals:
    
public slots:
    
};

#endif // PACKAGETREEMODEL_H
