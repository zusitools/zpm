#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItem>
#include <QItemSelection>
#include <QtXml/QDomElement>
#include <QMap>
#include <QPair>
#include "package.h"
#include "packagetreemodel.h"
#include "packagetreesortfilterproxymodel.h"
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

    PackageTreeModel *packageTreeModel;
    PackageTreeSortFilterProxyModel *sortFilterProxyModel;

    void nodeToItem(const QDomNode &node, QStandardItem *parent);

    // Solver information
    QMap<QPair<PackageVersion*, PackageFile*>, int> dependencyRules;
    QMap<QPair<PackageVersion*, PackageVersion*>, int> conflictRules;
    QMap<Package*, int> policyRules;
    QMap<PackageVersion*, int> jobRules;

    QMap<int, int*> disabledClausesByLiteralCount;
    QMap<int, int*> disabledClauses;

    // This assigns a variable number to each package version.
    QMap<PackageVersion *, int> variables;
    Cnf *cnf;



    int variableNumber(PackageVersion *version);
    void solve();

    template <class T> void disableRule(QMap<T, int> ruleMap, T key);
    template <class T> void enableRule(QMap<T, int> ruleMap, T key);
};

#endif // MAINWINDOW_H
