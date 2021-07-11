#ifndef MATERIALTRACKER_H
#define MATERIALTRACKER_H

#include "WidgetsHeader/MaterialTracker/MaterialStatusBar.h"
#include "NonWidgetsHeader/ErrorHandling.h"
#include "NonWidgetsHeader/SaveAndLoad.h"
#include "NonWidgetsHeader/ApiAccess.h"
#include "NonWidgetsHeader/DataInterface.h"
#include <QMenu>
#include <QString>
#include <QInputDialog>
#include <QDebug>
#include <QTabBar>
#include <QWidget>
#include <QVector>
#include <QCompleter>

namespace Ui { class MaterialTracker; }

class MaterialTracker : public QWidget
{
    Q_OBJECT

public:
    MaterialTracker(QWidget *parent = nullptr);
    ~MaterialTracker();

    // --- Change status label ---
    void ChangeWaitingForApiReplyStatusLabel();

    // --- Tab names ---
    QVector<QString> GetAllTabNames();
    QString getTabName() const { return tabName; }
    void setTabName(const QString& value) { tabName = value; }

    // --- Search Recipe ---
    void SearchRecipe();
    void RecursiveSearchRecipe(QString recipeName);

    // --- Importing ---
    void ImportExcelSheet();

public slots:

    // I/O
    void SaveFile();
    void SaveAsFile();
    void OpenFile();

private slots:
    // Tabs
    void AddTab();
    void RemoveTab(int index);
    void RenameTab(int index);

    // API
    void UpdateMaterials();

    // Sorting
	void SortingConfig(int index);
    void SortByCategory();
    void SortByName();
    void SortByCurrent();
    void SortByGoal();
    void SortByPercent();

private:
    Ui::MaterialTracker* ui;

    // --- Styling ---
    void SetIcons();

    // --- Error Handling ---
    ErrorHandling error;

    // --- Save and Load ---
    SaveAndLoad* saveAndLoad;

    // --- Data Structures ---
    QVector<MaterialStatusBar*> materialTabs;

    // --- Variables ---
    QString tabName;

    // --- Search Recipe ---
    QVector<QString> resultName;
    QVector<int> resultCount;

    // --- Sorting ---
    void ResetSortingButtons();

    // --- Tabs ---
    void CreateTabDataTable(QString name);
    void DeleteTabDataTable(QString name);
    void DeleteAllTabDataTables();

    void CreateTabsFromFile(int tabAmt,
                            const QVector<QString>& tabNames,
                            const QVector<int>& amtOfMatsInTab,
                            const QVector<QVector<QString>>& namesInTab,
                            const QVector<QVector<int>>& currentAmtsInTab,
                            const QVector<QVector<int>>& goalAmtsInTab);

   // --- Change status label ---
   void CheckSaveFileStatus(int result); // TODO move to Error Handling
   void ChangeLoadingFileStatusLabel();
   void ChangeMaterialUpdatingStatusLabel();

   // --- Word Completer ---
   QString searchedRecipe;
   QStringList recipeList;
   QCompleter* wordCompleter;
   void SetupRecipeWordCompleter();
};

#endif // MATERIALTRACKER_H
