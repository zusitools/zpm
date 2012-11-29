#ifndef PACKAGEITEMDELEGATE_H
#define PACKAGEITEMDELEGATE_H

#include <QItemDelegate>

class PackageItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit PackageItemDelegate(QObject *parent = 0);
    void drawCheck(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, Qt::CheckState state) const;
    
signals:
    
public slots:
    
};

#endif // PACKAGEITEMDELEGATE_H
