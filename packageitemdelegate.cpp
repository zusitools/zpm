#include "packageitemdelegate.h"
#include <QPainter>
#include <QDebug>

PackageItemDelegate::PackageItemDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

void PackageItemDelegate::drawCheck(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, Qt::CheckState state) const
{
    if (state == Qt::Unchecked)
    {
        QIcon("icons/noinst.xpm").paint(painter, rect);
    }
    else
    {
        QIcon("icons/keepinstalled.xpm").paint(painter, rect);
    }
}
