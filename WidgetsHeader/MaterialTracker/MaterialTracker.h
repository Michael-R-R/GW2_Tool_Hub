#ifndef MATERIALTRACKER_H
#define MATERIALTRACKER_H

#include "WidgetsHeader/MaterialTracker/MaterialStatusBar.h"
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

public:
    void ChangeWaitingForApiReplyStatusLabel();

    QVector<QString> GetAllTabNames();
    QString getTabName() const { return tabName; }
    void setTabName(const QString &value) { tabName = value; }

private:
    Ui::MaterialTracker* ui;
    QDir iconDir;

    QVector<MaterialStatusBar*> materialTabs;

    // --- Variables ---
    QString tabName;

    // --- Functions ---
    void CreateTabDataTable(QString tabName);
    void DeleteTabDataTable(QString tabName);
    void DeleteAllTabDataTables();
    void CreateTabsFromFile(int tabAmt,
                            QVector<QString> tabNames,
                            QVector<int> amtOfMatsInTab,
                            QVector<QVector<QString>> namesInTab,
                            QVector<QVector<int>> currentAmtsInTab,
                            QVector<QVector<int>> goalAmtsInTab);

   void CheckSaveFileStatus(int result);

   void ChangeLoadingFileStatusLabel();
   void ChangeMaterialUpdatingStatusLabel();
};

#endif // MATERIALTRACKER_H
