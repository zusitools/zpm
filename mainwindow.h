#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItem>
#include <QItemSelection>
#include <QtXml/QDomElement>
#include "package.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void loadRepoData();
    
private slots:
    void on_treeView_customContextMenuRequested(const QPoint &pos);
    void packageCheckStateChanged(QModelIndex index);
    void treeViewSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);

private:
    Ui::MainWindow *ui;
    QList<QStandardItem *> prepareRow(const QString &first,
                                                    const QString &second,
                                                    const QString &third);
    void nodeToItem(const QDomNode &node, QStandardItem *parent);
};

#endif // MAINWINDOW_H
