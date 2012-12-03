#ifndef PACKAGEITEMDELEGATE_H
#define PACKAGEITEMDELEGATE_H

#include <QStyledItemDelegate>

class PackageItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit PackageItemDelegate(QObject *parent = 0);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    
signals:
    
public slots:
    
};

#endif // PACKAGEITEMDELEGATE_H
