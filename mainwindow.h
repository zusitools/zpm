#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItem>
#include <QItemSelection>
#include <QtXml/QDomElement>
#include "package.h"
#include "tinisat/Cnf.h"

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

    void on_lineEdit_textChanged(const QString &newValue);

    void test_slot(QAction *action);

private:
    Ui::MainWindow *ui;
    QMap<QString, Package *> packages;
    void nodeToItem(const QDomNode &node, QStandardItem *parent);

    // Solver information

    // This assigns a variable number to each package version.
    QMap<PackageVersion *, int> variables;
    Cnf *cnf;

    int variableNumber(PackageVersion *version);
    void solve();
};

#endif // MAINWINDOW_H
