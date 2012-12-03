#include "packageitemdelegate.h"
#include "packagetreeitem.h"
#include "packagetreesortfilterproxymodel.h"
#include "packageitem.h"
#include <QPainter>
#include <QApplication>
#include <QDebug>

PackageItemDelegate::PackageItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void PackageItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    if (index.isValid() && index.column() == 0) {
        PackageTreeSortFilterProxyModel *proxyModel = (PackageTreeSortFilterProxyModel*)(index.model());
        QModelIndex realIndex = proxyModel->mapToSource(index);

        if (((PackageTreeItem*)(realIndex.internalPointer()))->getType() == PACKAGE) {
            PackageItem *package = (PackageItem*)(realIndex.internalPointer());
            QRect rect = QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator, &option);

            // FIXME
            rect.setWidth(22);
            rect.setHeight(16);


            QIcon("icons/noinst.xpm").paint(painter, rect);

            switch (package->state()) {
                case AUTODELETE:
                    QIcon("icons/autodel.xpm").paint(painter, rect);
                    break;
                case AUTOINSTALL:
                    QIcon("icons/autoinstall.xpm").paint(painter, rect);
                    break;
                case AUTOUPDATE:
                    QIcon("icons/autoupdate.xpm").paint(painter, rect);
                    break;
                case DELETE:
                    QIcon("icons/del.xpm").paint(painter, rect);
                    break;
                case INSTALL:
                    QIcon("icons/install.xpm").paint(painter, rect);
                    break;
                case KEEPINSTALLED:
                    QIcon("icons/keepinst.xpm").paint(painter, rect);
                    break;
                case NOTINSTALLED:
                    break;
                case PROTECTED:
                    QIcon("icons/protected.xpm").paint(painter, rect);
                    break;
                case TABOO:
                    QIcon("icons/taboo.xpm").paint(painter, rect);
                    break;
                case UPDATE:
                    QIcon("icons/update.xpm").paint(painter, rect);
                    break;
            }
        }
    }


}
