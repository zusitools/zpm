#ifndef PACKAGEMODEL_H
#define PACKAGEMODEL_H

#include <QStandardItemModel>

class PackageModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit PackageModel(QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // PACKAGEMODEL_H
