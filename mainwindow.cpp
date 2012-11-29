#include <QtGui>
#include <QStandardItemModel>
#include <QStandardItem>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    rowItems.first()->setIcon(QIcon("/opt/kde3/share/icons/Crystal Clear/16x16/filesystems/folder_blue.png"));
    return rowItems;
}


void MainWindow::action_load_xml_triggered()
{
    QFile* xmlFile = new QFile("/home/jojo/dokumente/projekte/zusi/zpm/repo.xml");
    if (!xmlFile->open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, tr("Unable to open file"), xmlFile->errorString());
        return;
    }

    QXmlStreamReader reader(xmlFile);

    // XML-Datei lesen und in Nodes umwandeln

    QStandardItemModel *model = new QStandardItemModel();

    QList<QStandardItem *> preparedRow = prepareRow("first", "second", "third");
    QStandardItem *item = model->invisibleRootItem();
    // adding a row to the invisible root item produces a root element
    item->appendRow(preparedRow);

    QList<QStandardItem *> secondRow = prepareRow("111", "222", "333");
    // adding a row to an item starts a subtree
    preparedRow.first()->appendRow(secondRow);

    model->setHeaderData(0, Qt::Horizontal, tr("Name"));
    model->setHeaderData(1, Qt::Horizontal, tr("Installierte Version"));
    model->setHeaderData(2, Qt::Horizontal, tr("Verfügbare Version"));
    model->setHeaderData(3, Qt::Horizontal, tr("Autor"));

    ui->treeView->setModel(model);
    ui->treeView->expandAll();
}

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    // TODO
}
