#ifndef PACKAGETREESORTFILTERPROXYMODEL_H
#define PACKAGETREESORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class PackageTreeSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit PackageTreeSortFilterProxyModel(QObject *parent = 0);

    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    
signals:
    
public slots:
    
};

#endif // PACKAGETREESORTFILTERPROXYMODEL_H
