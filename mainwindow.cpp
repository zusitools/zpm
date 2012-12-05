#include <QtGui>
#include <QDebug>
#include <QRegExp>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "packagetreemodel.h"
#include "packagetreesortfilterproxymodel.h"
#include "packageitemdelegate.h"
#include "packageitem.h"
#include "package.h"
#include "packagefile.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->treeView->header()->setResizeMode(0, QHeaderView::Stretch);
    ui->treeView->header()->setResizeMode(1, QHeaderView::Custom);
    ui->treeView->header()->setResizeMode(2, QHeaderView::Custom);

    ui->treeView->setItemDelegate(new PackageItemDelegate());

    this->loadRepoData();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadRepoData()
{
    QTime myTimer;
    myTimer.start();

    // Read XML files
    QStringList xmlFiles;
    xmlFiles << "repo_gt8.xml";
    xmlFiles << "repo_langeland.xml";
    xmlFiles << "repo_driburg.xml";
    xmlFiles << "repo_altenbeken.xml";
    xmlFiles << "repo_timetable.xml";

    QMap<QString, PackageFile *> fileNames;

    foreach (QString fileName, xmlFiles) {
        QFile *xmlFile = new QFile(fileName);
        if (!xmlFile->open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this, tr("Unable to open file"), xmlFile->errorString());
            continue;
        }

        QDomDocument xmlDoc;

        if (!xmlDoc.setContent(xmlFile)) {
            // TODO error handling
            continue;
        }

        // FIXME hack
        QDomNodeList packageNodes = xmlDoc.documentElement().childNodes();
        for (int i = 0; i < packageNodes.count(); i++) {
            if (packageNodes.at(i).nodeName() == "package") {
                Package *package = new Package(packageNodes.at(i).attributes().namedItem("name").nodeValue(),
                                               packageNodes.at(i).attributes().namedItem("displayname").nodeValue(),
                                               qrand() % 2 == 0 ? NOTINSTALLED : KEEPINSTALLED);
                packages.append(package);

                QList<PackageFile *> fileList;
                PackageFile* file;

                for (int j = 0; j < packageNodes.at(i).childNodes().at(0).childNodes().count(); j++) {
                    QDomNode node = packageNodes.at(i).childNodes().at(0).childNodes().at(j);
                    QString fileName = node.attributes().namedItem("path").nodeValue();

                    if (!fileNames.contains(fileName)) {
                        file = new PackageFile(fileName);
                        fileNames.insert(fileName, file);
                    } else {
                        file = fileNames[fileName];
                    }

                    if (node.nodeName() == "file") {
                        fileList << file;
                    } else if (node.nodeName() == "dependency") {
                        package->addDependency(file);
                    }
                }

                PackageVersion *version = new PackageVersion(fileList, package, fileName);
                package->appendVersion(version);
                package->setInstalledVersion(version);

                for (int j = 0; j < fileList.count(); j++) {
                    fileList.at(j)->addProvider(version);
                }
            }
        }

        xmlFile->close();
    }

    PackageTreeModel *treeModel = new PackageTreeModel(&packages);
    PackageTreeSortFilterProxyModel *sortFilterModel = new PackageTreeSortFilterProxyModel();
    sortFilterModel->setSourceModel(treeModel);

    ui->treeView->setModel(sortFilterModel);
    connect(ui->treeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(treeViewSelectionChanged(QItemSelection,QItemSelection)));
    connect(treeModel, SIGNAL(packageCheckStateChanged(QModelIndex)), this, SLOT(packageCheckStateChanged(QModelIndex)));

    ui->treeView->sortByColumn(0, Qt::AscendingOrder);
    ui->treeView->expandAll();

    ui->treeView->resizeColumnToContents(0);
    ui->treeView->resizeColumnToContents(1);
    ui->treeView->resizeColumnToContents(2);
    ui->treeView->resizeColumnToContents(3);

    qDebug() << "Loading model took " + QString::number(myTimer.elapsed()) + " ms";
}

void MainWindow::on_treeView_customContextMenuRequested(const QPoint &pos)
{
    // Retrieve model index of item at the position the context menu was opened
    QModelIndex itemIndex = ui->treeView->indexAt(pos);

    QMenu *menu = new QMenu;
    menu->addAction(QIcon(":/zpm/icons/keepinstalled.xpm"), tr("Keep"), this, SLOT(test_slot()));
    menu->addAction(QIcon(":/zpm/icons/del.xpm"), tr("Delete"), this, SLOT(test_slot()));
    menu->addAction(QIcon(":/zpm/icons/update.xpm"), tr("Update"), this, SLOT(test_slot()));
    menu->addAction(QIcon(":/zpm/icons/update.xpm"), tr("Force update"), this, SLOT(test_slot()));
    menu->exec(ui->treeView->mapToGlobal(pos));
}

void MainWindow::packageCheckStateChanged(QModelIndex index)
{
    Package *package = ((PackageItem*)index.internalPointer())->package();

    switch (package->state()) {
        case KEEPINSTALLED:
            package->setState(UPDATE);
            break;
        case UPDATE:
            package->setState(DELETE);
            break;
        case DELETE:
            package->setState(package->originalState());
            break;
        case NOTINSTALLED:
            package->setState(INSTALL);
            break;
        case INSTALL:
            package->setState(package->originalState());
            break;
        default:
            break;
    }
}

void MainWindow::treeViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);

    ui->infoTextEdit->clear();

    if (selected.count() == 1) {
        QModelIndex index = selected.at(0).indexes().at(0);
        PackageTreeSortFilterProxyModel *proxyModel = (PackageTreeSortFilterProxyModel*) index.model();
        PackageTreeItem *item = (PackageTreeItem*)(proxyModel->mapToSource(index).internalPointer());
        if (item->getType() == PACKAGE) {
            Package *package = ((PackageItem*)item)->package();
            QString text = package->getQualifiedName() + "\n\nFiles:";

            for (int i = 0; i < package->installedVersion()->files()->count(); i++) {
                text += "\n" + package->installedVersion()->files()->at(i)->name();
            }

            text += "\n\nDependencies:";

            for (int i = 0; i < package->dependencies().count(); i++) {
                text += "\n" + package->dependencies().at(i)->name();

                for (int j = 0; j < package->dependencies().at(i)->providers()->count(); j++) {
                    text += "\n     provided by " + package->dependencies().at(i)->providers()->at(j)->package()->getQualifiedName()
                            + " in repo " + package->dependencies().at(i)->providers()->at(j)->repo();
                }

            }

            ui->infoTextEdit->setPlainText(text);
        }
    }
}

void MainWindow::on_lineEdit_textChanged(const QString &newValue)
{
    ((PackageTreeSortFilterProxyModel* )(ui->treeView->model()))->setFilterRegExp(QRegExp(newValue, Qt::CaseInsensitive));

    if (!newValue.isEmpty()) {
        ui->treeView->expandAll();
    }
}
