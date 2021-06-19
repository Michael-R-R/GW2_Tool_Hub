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

namespace Ui { class MaterialTracker; }

class MaterialTracker : public QWidget
{
    Q_OBJECT

public:
    MaterialTracker(QWidget *parent = nullptr);
    ~MaterialTracker();

public slots:
    void SaveToFile();
    void LoadFromFile();

private slots:
    void AddTab();
    void RemoveTab(int index);
    void RenameTab(int index);
    void UpdateMaterials();

    // Sorting
    void SortByCategory();
    void SortByName();
    void SortByCurrent();
    void SortByGoal();
    void SortByPercent();

public:
    void ChangeWaitingForApiReplyStatusLabel();

    // Tab names
    QVector<QString> GetAllTabNames();
    QString getTabName() const { return tabName; }
    void setTabName(const QString &value) { tabName = value; }

    // Importing
    void ImportExcelSheet();

private:
    Ui::MaterialTracker* ui;
    QDir iconDir;

    // --- Error Handling ---
    ErrorHandling error;

    // --- Data Structures ---
    QVector<MaterialStatusBar*> materialTabs;

    // --- Variables ---
    QString tabName;

    // --- Functions ---
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
};

#endif // MATERIALTRACKER_H
