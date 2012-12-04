#include <QtGui>
#include <QDebug>
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
    QList<QFile*> xmlFiles;
    xmlFiles << new QFile("repo_gt8.xml");
    xmlFiles << new QFile("repo_langeland.xml");
    xmlFiles << new QFile("repo_driburg.xml");
    xmlFiles << new QFile("repo_altenbeken.xml");
    xmlFiles << new QFile("repo_timetable.xml");

    // TODO this has to be deleted at some point
    QList<Package *> *packages = new QList<Package *>();

    foreach (QFile* xmlFile, xmlFiles) {
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
                packages->append(new Package(packageNodes.at(i).attributes().namedItem("name").nodeValue(),
                                             packageNodes.at(i).attributes().namedItem("displayname").nodeValue(),
                                             qrand() % 2 == 0 ? NOTINSTALLED : KEEPINSTALLED));
            }
        }

        xmlFile->close();
    }

    PackageTreeModel *treeModel = new PackageTreeModel(packages);
    PackageTreeSortFilterProxyModel *sortFilterModel = new PackageTreeSortFilterProxyModel();
    sortFilterModel->setSourceModel(treeModel);

    connect(treeModel, SIGNAL(packageCheckStateChanged(QModelIndex)), this, SLOT(packageCheckStateChanged(QModelIndex)));

    ui->treeView->setModel(sortFilterModel);
    ui->treeView->sortByColumn(0, Qt::AscendingOrder);
    ui->treeView->expandAll();

    ui->treeView->resizeColumnToContents(0);
    ui->treeView->resizeColumnToContents(1);
    ui->treeView->resizeColumnToContents(2);
    ui->treeView->resizeColumnToContents(3);

    qDeleteAll(xmlFiles);

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
