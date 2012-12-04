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

            switch (package->state()) {
                case AUTODELETE:
                    QIcon(":/zpm/icons/autodel.xpm").paint(painter, rect);
                    break;
                case AUTOINSTALL:
                    QIcon(":/zpm/icons/autoinstall.xpm").paint(painter, rect);
                    break;
                case AUTOUPDATE:
                    QIcon(":/zpm/icons/autoupdate.xpm").paint(painter, rect);
                    break;
                case DELETE:
                    QIcon(":/zpm/icons/del.xpm").paint(painter, rect);
                    break;
                case INSTALL:
                    QIcon(":/zpm/icons/install.xpm").paint(painter, rect);
                    break;
                case KEEPINSTALLED:
                    QIcon(":/zpm/icons/keepinstalled.xpm").paint(painter, rect);
                    break;
                case NOTINSTALLED:
                    QIcon(":/zpm/icons/noinst.xpm").paint(painter, rect);
                    break;
                case PROTECTED:
                    QIcon(":/zpm/icons/protected.xpm").paint(painter, rect);
                    break;
                case TABOO:
                    QIcon(":/zpm/icons/taboo.xpm").paint(painter, rect);
                    break;
                case UPDATE:
                    QIcon(":/zpm/icons/update.xpm").paint(painter, rect);
                    break;
            }
        }
    }


}
