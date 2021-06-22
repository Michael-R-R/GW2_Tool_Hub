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

    // I/O
    void SaveToFile();
    void LoadFromFile();

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

    // --- Data Structures ---
    QVector<MaterialStatusBar*> materialTabs;

    // --- Variables ---
    QString tabName;

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

public:
    // --- Change status label ---
    void ChangeWaitingForApiReplyStatusLabel();

	// --- Tab names ---
	QVector<QString> GetAllTabNames();
	QString getTabName() const { return tabName; }
	void setTabName(const QString& value) { tabName = value; }

	// --- Importing ---
	void ImportExcelSheet();
};

#endif // MATERIALTRACKER_H
