#include <QtGui>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QSortFilterProxyModel>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>
#include <QXmlQuery>
#include <QXmlResultItems>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "packagetreemodel.h"
#include "packagetreesortfilterproxymodel.h"
#include "packageitemdelegate.h"
#include <iostream>

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

    QXmlQuery query;
    query.setQuery("/root/package");

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
        // TODO duplicate filtering
        QDomNodeList packageNodes = xmlDoc.documentElement().childNodes();//.at(0).childNodes();
        for (int i = 0; i < packageNodes.count(); i++) {
            if (packageNodes.at(i).nodeName() == "package") {
                packages->append(new Package(packageNodes.at(i).attributes().namedItem("name").nodeValue()));
            }
        }

        xmlFile->close();
    }

    PackageTreeModel *treeModel = new PackageTreeModel(packages);
    PackageTreeSortFilterProxyModel *sortFilterModel = new PackageTreeSortFilterProxyModel();
    sortFilterModel->setSourceModel(treeModel);

    ui->treeView->setModel(sortFilterModel);
    ui->treeView->sortByColumn(0, Qt::AscendingOrder);
    ui->treeView->expandAll();

    ui->treeView->resizeColumnToContents(0);
    ui->treeView->resizeColumnToContents(1);
    ui->treeView->resizeColumnToContents(2);
    ui->treeView->resizeColumnToContents(3);

    // connect(model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(treeViewItemChanged(QStandardItem*)));

    qDebug() << "Loading model took " + QString::number(myTimer.elapsed()) + " ms";
}

void MainWindow::on_treeView_customContextMenuRequested(const QPoint &pos)
{
    // Retrieve model index of item at the position the context menu was opened
    QModelIndex itemIndex = ui->treeView->indexAt(pos);

    QMenu *menu = new QMenu;
    menu->addAction(QString::number(itemIndex.row()), this, SLOT(test_slot()));
    menu->exec(ui->treeView->mapToGlobal(pos));
}

void MainWindow::treeViewItemChanged(QStandardItem *item)
{
    // TODO react to change of selection
    using namespace std;
    cout << item->text().toStdString();
}
