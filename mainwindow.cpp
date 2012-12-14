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
#include "tinisat/Cnf.h"
#include "tinisat/SatSolver.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QList<int> sizes;
    sizes << 150;
    sizes << 850;
    ui->horizontalSplitter->setSizes(sizes);

    sizes.clear();
    sizes << 750;
    sizes << 250;
    ui->verticalSplitter->setSizes(sizes);

    ui->treeView->setItemDelegate(new PackageItemDelegate());

    this->loadRepoData();
}

MainWindow::~MainWindow()
{
    delete ((PackageTreeSortFilterProxyModel*) ui->treeView->model())->sourceModel();
    delete ui->treeView->itemDelegate();
    delete ui;
    delete cnf;
}

int MainWindow::variableNumber(PackageVersion *version) {
    if (!variables.contains(version)) {
        int index = variables.count() + 2; // indices must start at 1, literal 1 is reserved for disabling clauses!
        variables.insert(version, index);
        return index;
    } else {
        return variables[version];
    }
}

#define DEBUGCLAUSE() QString dbg = QString(""); for (unsigned int xx = 0; xx < clause_lengths[clause_index]; xx++) { dbg += QString::number(clauses[clause_index][xx]) + "(" + variables.key(qAbs(clauses[clause_index][xx]))->package()->getDisplayName() + "@" + variables.key(qAbs(clauses[clause_index][xx]))->repo() + ") | "; } qDebug() << dbg;

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

    // Total number of dependencies (needed for clause count)
    int dependency_count = 0;
    int provider_count = 0;
    int installed_count = 0;

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
                Package *package;
                QString packageName = packageNodes.at(i).attributes().namedItem("name").nodeValue();

                if (!packages.contains(packageName)) {
                    package = new Package(packageName, packageNodes.at(i).attributes().namedItem("displayname").nodeValue());
                    packages.insert(packageName, package);

                    if (package->state() == KEEPINSTALLED) {
                        installed_count++;
                    }
                } else {
                    package = packages[packageName];
                }

                QList<PackageFile *> fileList;
                QList<PackageFile *> dependencyList;
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
                        dependencyList << file;
                        dependency_count++;
                    }

                    fileName.clear();
                }

                PackageVersion *version = new PackageVersion(fileList, package, fileName, false);

                foreach (PackageFile *dependency, dependencyList) {
                    version->addDependency(dependency);
                }

                package->appendVersion(version);

                for (int j = 0; j < fileList.count(); j++) {
                    fileList.at(j)->addProvider(version);
                    provider_count++;
                }
            }
        }

        xmlFile->close();
        delete xmlFile;
    }

    xmlFiles.clear();

    QList<Package *> packageValues = packages.values();
    packageTreeModel = new PackageTreeModel(&packageValues);
    sortFilterProxyModel = new PackageTreeSortFilterProxyModel();
    sortFilterProxyModel->setSourceModel(packageTreeModel);

    ui->treeView->setModel(sortFilterProxyModel);
    connect(ui->treeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(treeViewSelectionChanged(QItemSelection,QItemSelection)));
    connect(packageTreeModel, SIGNAL(packageCheckStateChanged(QModelIndex)), this, SLOT(packageCheckStateChanged(QModelIndex)));

    ui->treeView->sortByColumn(0, Qt::AscendingOrder);
    ui->treeView->expandAll();

    ui->treeView->resizeColumnToContents(0);
    ui->treeView->resizeColumnToContents(1);
    ui->treeView->resizeColumnToContents(2);
    ui->treeView->resizeColumnToContents(3);

    qDebug() << "Loading model took " + QString::number(myTimer.elapsed()) + " ms";
    myTimer.restart();

    // we overestimate the number of conflicts clauses but that's OK
    // +1 because of the dummy clause containing literal 1
    unsigned clause_count = dependency_count + provider_count + installed_count + 1;

    unsigned literal_count = 0;
    unsigned clause_index = 0;
    unsigned *clause_lengths = (unsigned*) calloc(clause_count, sizeof(unsigned));

    int** clauses = (int **) calloc(clause_count, sizeof(int *));

    if (clauses == NULL) {
        QMessageBox::critical(this, tr("Error"), tr("Cannot allocate memory for dependency resolution"));
        exit(1);
    }

    // There has to be at least one clause containing literal 1, else the SAT solver will crash.
    // Insert a clause which contains only the literal 1
    clause_lengths[clause_index] = 1;
    clauses[clause_index] = (int*) calloc(2, sizeof(int));
    clauses[clause_index][0] = 1;
    literal_count++;
    clause_index++;

    // dependency clauses
    foreach (Package *package, packages) {
        foreach (PackageVersion *version, *(package->versions())) {
            foreach (PackageFile *dependency, version->dependencies()) {

                clause_lengths[clause_index] = dependency->providers()->count() + 1;
                clauses[clause_index] = (int*)calloc(clause_lengths[clause_index] + 1, sizeof(int));

                clauses[clause_index][0] = -variableNumber(version);
                literal_count++;

                for (int i = 0; i < dependency->providers()->count(); i++) {
                    clauses[clause_index][i + 1] = variableNumber(dependency->providers()->at(i));
                    literal_count++;
                }

                dependencyRules.insert(QPair<PackageVersion*, PackageFile*>(version, dependency), clause_index);

                //qDebug() << "dependency" << dependency->name();
                //DEBUGCLAUSE()

                clause_index++;
            }
        }
    }

    qDebug() << "variable count" << variables.count();
    qDebug() << "clause count" << clause_index;
    qDebug() << "literal count" << literal_count;
    qDebug();

    // conflicts clauses
    foreach (PackageFile *file, fileNames.values()) {
        if (file->providers()->count() <= 2) {
            continue;
        }

        for (int i = 0; i < file->providers()->count() - 1; i++) {
            int index_i = variableNumber(file->providers()->at(i));

            for (int j = i + 1; j < file->providers()->count(); j++) {
                literal_count += 2;
                clause_lengths[clause_index] = 2;
                clauses[clause_index] = (int*)calloc(2 + 1, sizeof(int));

                if (clauses[clause_index] == NULL) {
                    QMessageBox::critical(this, tr("Error"), tr("Cannot allocate memory for dependency resolution"));
                    exit(1);
                }

                clauses[clause_index][0] = -index_i;
                clauses[clause_index][1] = -variableNumber(file->providers()->at(j));

                conflictRules.insert(QPair<PackageVersion*, PackageVersion*>(file->providers()->at(i), file->providers()->at(j)), clause_index);

                //qDebug() << "conflict" << file->name();
                //DEBUGCLAUSE()

                clause_index++;
            }
        }
    }

    qDebug() << "variable count" << variables.count();
    qDebug() << "clause count" << clause_index;
    qDebug() << "literal count" << literal_count;
    qDebug();

    // TODO policy rules (keep installed packages => conflict when removing packages)
    // Policy rules can be removed (negating all of their literals? using a special literal?)
    foreach (Package *package, packages) {
        if (package->state() != KEEPINSTALLED) {
            continue;
        }

        clause_lengths[clause_index] = package->versions()->count();
        literal_count += clause_lengths[clause_index];
        clauses[clause_index] = (int*)calloc(clause_lengths[clause_index] + 1, sizeof(int));

        if (clauses[clause_index] == NULL) {
            QMessageBox::critical(this, tr("Error"), tr("Cannot allocate memory for dependency resolution"));
            exit(1);
        }

        for (int i = 0; i < package->versions()->count(); i++) {
            clauses[clause_index][i] = variableNumber(package->versions()->at(i));
        }

        policyRules.insert(package, clause_index);

        //qDebug() << "policy" << package->getQualifiedName();
        //DEBUGCLAUSE()

        clause_index++;
    }

    clauses = (int **) realloc(clauses, clause_index * sizeof(int *));

    qDebug() << "variable count" << variables.count();
    qDebug() << "clause count" << clause_index;
    qDebug() << "literal count" << literal_count;

    cnf = new Cnf(variables.count() + 1, clause_index, clauses, literal_count, clause_lengths);
    qDebug() << "Creating dependencies took " + QString::number(myTimer.elapsed()) + " ms";

    myTimer.restart();
    solve();

    qDebug() << "Solving took " + QString::number(myTimer.elapsed()) + " ms";
}

void MainWindow::solve() {
    SatSolver solver(*cnf);

    if (solver.run()) {
        int vc;
        int* values;

        solver.getSolution(vc, values);

        for (int i = 2; i < vc; i++) {
            if (i < 10) qDebug() << values[i];

            PackageVersion *version = variables.key(i);

            // TODO: update tree view, differentiate between manual and automatic changes
            if (values[i] < 0) {
                version->setKeepInstalled(false);
            } else {
                version->setKeepInstalled(true);
            }
        }

        free(values);

        solver.printStats();
    }
}

void MainWindow::insertJobRule(PackageVersion *version, char sign) {
    if (jobRules.contains(version)) {
        if (disabledClauses.contains(jobRules[version])) {
            enableRule(jobRules, version);
        }

        cnf->clauses[jobRules[version]][0] = sign * variableNumber(version);
    } else {
        int* lit = (int*) calloc(2, sizeof(int));
        lit[0] = sign * variableNumber(version);
        lit[1] = 0;

        jobRules.insert(version, cnf->addClauseWithExistingVars(lit, 1));
    }
}

void MainWindow::installPackage(Package *package)
{
    installPackageVersion(package->versions()->first());
}

void MainWindow::installPackageVersion(PackageVersion *packageVersion)
{
    // TODO: do not install other versions of this package?
    packageVersion->package()->setAutoChange(false);
    insertJobRule(packageVersion, +1);
    solve();
}

void MainWindow::keepPackage(Package *package)
{
    foreach (PackageVersion *version, *(package->versions())) {
        if (jobRules.contains(version) && !disabledClauses.contains(jobRules[version])) {
            disableRule(jobRules, version);
        }
    }

    solve();
}

void MainWindow::deletePackage(Package *package)
{
    // TODO undo any other operation on this package
    if (package->installedVersion() != NULL) {
        insertJobRule(package->installedVersion(), -1);
        solve();
    }
}

void MainWindow::protectPackage(Package *package)
{

}

void MainWindow::on_treeView_customContextMenuRequested(const QPoint &pos)
{
    // Retrieve model index of item at the position the context menu was opened
    QModelIndex itemIndex = ui->treeView->indexAt(pos);

    PackageTreeItem *item = (PackageTreeItem*)(sortFilterProxyModel->mapToSource(itemIndex).internalPointer());

    if (item->getType() == PACKAGE) {
        Package *package = ((PackageItem*)item)->package();
        QMenu *menu = new QMenu;

        switch (package->originalState()) {
            case NOTINSTALLED: {
                QAction *installAction = new QAction(QIcon(":/zpm/icons/install.xpm"), tr("Install"), this);
                installAction->setProperty("action", "install");
                installAction->setData(qVariantFromValue((void*)package));
                menu->addAction(installAction);

                QAction *tabooAction = new QAction(QIcon(":/zpm/icons/taboo.xpm"), tr("Taboo -- Never install"), this);
                tabooAction->setProperty("action", "taboo");
                tabooAction->setData(qVariantFromValue((void*)package));
                menu->addAction(tabooAction);

                QAction *noinstAction = new QAction(QIcon(":/zpm/icons/noinst.xpm"), tr("Do Not install"), this);
                noinstAction->setProperty("action", "noinst");
                noinstAction->setData(qVariantFromValue((void*)package));
                menu->addAction(noinstAction);

                break;
            }

            case KEEPINSTALLED:
            case PROTECTED:
                menu->addAction(QIcon(":/zpm/icons/keepinstalled.xpm"), tr("Keep"), this, SLOT(test_slot(QAction *)));
                menu->addAction(QIcon(":/zpm/icons/del.xpm"), tr("Delete"), this, SLOT(test_slot(QAction *)));
                menu->addAction(QIcon(":/zpm/icons/update.xpm"), tr("Update"), this, SLOT(test_slot(QAction *)));
                menu->addAction(QIcon(":/zpm/icons/update.xpm"), tr("Update unconditionally"), this, SLOT(test_slot(QAction *)));
                menu->addAction(QIcon(":/zpm/icons/protected.xpm"), tr("Protected -- never modify"), this, SLOT(test_slot(QAction *)));
                break;
        }

        connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(test_slot(QAction*)));

        menu->exec(QCursor::pos());

        qDeleteAll(menu->actions());
        delete menu;
    }
}

void MainWindow::packageCheckStateChanged(QModelIndex index)
{
    Package *package = ((PackageItem*)index.internalPointer())->package();

    // Select an action based on the current state of the package.
    switch (package->state()) {
        case KEEPINSTALLED:
        case AUTOINSTALL:
            // update to latest version
            installPackageVersion(package->versions()->first()); // TODO
            break;
        case UPDATE:
            deletePackage(package);
            break;
        case DELETE:
            // keep installed version
            installPackageVersion(package->installedVersion());
            break;
        case NOTINSTALLED:
            // TODO which version to install?
            installPackageVersion(package->versions()->first());
            break;
        case INSTALL:
            // do not install package
            keepPackage(package);
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
        PackageTreeItem *item = (PackageTreeItem*) sortFilterProxyModel->mapToSource(index).internalPointer();
        if (item->getType() == PACKAGE) {
            Package *package = ((PackageItem*)item)->package();
            PackageVersion *version = package->versions()->first(); // TODO
            QString text = package->getQualifiedName() + "\n\nFiles:";

            for (int i = 0; i < version->files()->count(); i++) {
                text += "\n" + version->files()->at(i)->name();
            }

            text += "\n\nDependencies:";

            for (int i = 0; i < version->dependencies().count(); i++) {
                text += "\n" + version->dependencies().at(i)->name();

                for (int j = 0; j < version->dependencies().at(i)->providers()->count(); j++) {
                    text += "\n     provided by " + version->dependencies().at(i)->providers()->at(j)->package()->getQualifiedName()
                            + " in repo " + version->dependencies().at(i)->providers()->at(j)->repo()
                            + (version->dependencies().at(i)->providers()->at(j)->installed() ? " (X)" : "")
                            + (version->dependencies().at(i)->providers()->at(j)->keepInstalled() ? " (!)" : "");
                }
            }

            ui->infoTextEdit->setPlainText(text);
        }
    }
}

void MainWindow::on_lineEdit_textChanged(const QString &newValue)
{
    sortFilterProxyModel->setFilterRegExp(QRegExp(newValue, Qt::CaseInsensitive));

    if (!newValue.isEmpty()) {
        ui->treeView->expandAll();
    }
}

void MainWindow::test_slot(QAction *action)
{
    Package *package = (Package*)action->data().value<void*>();
    package->setAutoChange(false);

    if (action->property("action") == "install") {
        installPackage(package);
    } else if (action->property("action") == "noinst") {
        if (jobRules.contains(package->versions()->first())) {
            disableRule(jobRules, package->versions()->first());
            solve();
        }
    }
}


/**
 * @brief MainWindow::disableRule<T> Disables a rule by inserting a "dummy clause" instead of the clause belonging to that rule.
 * @param ruleMap The map containing the clause indices
 * @param key The key in the rule map
 */
template <class T>
void MainWindow::disableRule(QMap<T, int> ruleMap, T key)
{
    int clauseIndex = ruleMap[key];
    int clauseLength = cnf->cl[clauseIndex];

    // Save the pointer to the original clause
    disabledClauses[clauseIndex] = cnf->clauses[clauseIndex];

    // Retrieve a dummy clause with the same length as the original clause
    if (!disabledClausesByLiteralCount.contains(clauseLength)) {
        int* disabledClause = new int[clauseLength + 1];

        for (int i = 0; i < clauseLength; i++) {
            disabledClause[i] = 1;
        }
        disabledClause[clauseLength] = 0;

        disabledClausesByLiteralCount.insert(clauseLength, disabledClause);
    }

    // Insert dummy clause instead of the original clause.
    cnf->clauses[clauseIndex] = disabledClausesByLiteralCount[clauseLength];

}

template <class T>
void MainWindow::enableRule(QMap<T, int> ruleMap, T key)
{
    if (disabledClauses.contains(ruleMap[key])) {
        cnf->clauses[ruleMap[key]] = disabledClauses[ruleMap[key]];
        disabledClauses.remove(ruleMap[key]);
    }
}
