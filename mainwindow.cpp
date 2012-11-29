#include <QtGui>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "packagemodel.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->loadRepoData();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QList<QStandardItem *> MainWindow::prepareRow(const QString &first,
                                                const QString &second,
                                                const QString &third)
{
    QList<QStandardItem *> rowItems;
    rowItems << new QStandardItem(first);
    rowItems << new QStandardItem(second);
    rowItems << new QStandardItem(third);
    return rowItems;
}

void MainWindow::nodeToItem(const QDomNode &node, QStandardItem *parent) {
    if (node.nodeName() == "package")
    {
        QList<QStandardItem *> preparedRow = prepareRow(node.attributes().namedItem("name").nodeValue(), "bla", "bla");
        preparedRow.first()->setCheckable(true);
        parent->appendRow(preparedRow);
    }
    else if (node.nodeName() == "folder")
    {
        QList<QStandardItem *> preparedRow = prepareRow(node.attributes().namedItem("name").nodeValue(), QString::null, QString::null);
        preparedRow.first()->setIcon(QIcon("/opt/kde3/share/icons/Crystal Clear/16x16/filesystems/folder_blue.png"));
        parent->appendRow(preparedRow);

        QDomNode n = node.firstChild();
        while(!n.isNull()) {
            QDomElement e = n.toElement();
            if(!e.isNull()) {
                nodeToItem(n, preparedRow.first());
            }
            n = n.nextSibling();
        }
    }
    else if (node.nodeName() == "root" || node.nodeName() == "folder")
    {
        QDomNode n = node.firstChild();
        while(!n.isNull()) {
            QDomElement e = n.toElement();
            if(!e.isNull()) {
                nodeToItem(n, parent);
            }
            n = n.nextSibling();
        }
    }
}

void MainWindow::loadRepoData()
{
    // Read XML file
    // TODO: Read more than one XML file
    QFile* xmlFile = new QFile("/home/jojo/dokumente/projekte/zusi/zpm/repo.xml");
    if (!xmlFile->open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, tr("Unable to open file"), xmlFile->errorString());
        return;
    }

    // Parse XML file
    QDomDocument xmlDoc;
    if (!xmlDoc.setContent(xmlFile)) {
        // TODO error handling
        xmlFile->close();
        return;
    }

    xmlFile->close();

    PackageModel *model = new PackageModel(this);
    nodeToItem(xmlDoc.documentElement(), model->invisibleRootItem());

    ui->treeView->setModel(model);
    ui->treeView->expandAll();
}

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    // TODO
}
