#include "packagemodel.h"

PackageModel::PackageModel(QObject *parent) :
    QStandardItemModel(parent)
{
    // Set header texts
    QStringList headerTexts;
    headerTexts.append(tr("Name"));
    headerTexts.append(tr("Installed version"));
    headerTexts.append(tr("Available version"));
    headerTexts.append(tr("Author"));

    this->setHorizontalHeaderLabels(headerTexts);
}
