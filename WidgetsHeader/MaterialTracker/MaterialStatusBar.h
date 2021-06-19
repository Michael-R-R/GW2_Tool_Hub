#ifndef MATERIALSTATUSBAR_H
#define MATERIALSTATUSBAR_H

#include "WidgetsHeader/MaterialTracker/Materials.h"
#include "WidgetsHeader/MaterialTracker/MaterialSorting.h"
#include "NonWidgetsHeader/DataInterface.h"
#include <QWidget>
#include <QListWidgetItem>
#include <QThread>
#include <QVector>
#include <QCompleter>

namespace Ui { class MaterialStatusBar; }

class MaterialStatusBar : public QWidget
{
    Q_OBJECT

public:
    MaterialStatusBar(QWidget *parent = nullptr);
    ~MaterialStatusBar();

    // --- Functions ---
    void SetTabName(const QString& val) { tabName = val; }
    QString GetTabName() { return tabName; }
    void UpdateTabName(QString newTabName);

    int GetNumOfTrackedMaterials() { return trackedMaterials.size(); }
    QVector<QString> GetAllTrackedMaterialNames() const;
    QVector<int> GetAllTrackedCurrentAmounts() const;
    QVector<int> GetAllTrackedGoalAmounts() const;

    void UpdateTrackingStatus();
    void AddMaterialFromSaveFile(int amtOfMaterials,
                                 const QVector<QString>& matNames,
                                 const QVector<int>& currentAmts,
                                 const QVector<int>& goalAmts);
    void AddMaterialFromExcelFile(int matCount, QString matName);

    // --- Sorting ---
    // getters/setters
    int GetSortCategoryClicks() const { return sortCategoryClicks; };
    void SetSortCategoryClicks(int value) { sortCategoryClicks = value; }
    int GetSortNameClicks() const { return sortNameClicks; };
    void SetSortNameClicks(int value) { sortNameClicks = value; }
    int GetSortCurrentClicks() const { return sortCurrentClicks; };
    void SetSortCurrentClicks(int value) { sortCurrentClicks = value; }
    int GetSortGoalClicks() const { return sortGoalClicks; };
    void SetSortGoalClicks(int value) { sortGoalClicks = value; }
    int GetSortPercentClicks() const { return sortPercentClicks; };
    void SetSortPercentClicks(int value) { sortPercentClicks = value; }
    // no filter
    void SortNoFilter();
    // category
    void SortLowToHighCategory();
    void SortHighToLowCategory();
    // name
    void SortLowToHighName();
    void SortHighToLowName();
    // current
    void SortLowToHighCurrent();
    void SortHighToLowCurrent();
    // goal
    void SortLowToHighGoal();
    void SortHighToLowGoal();
    // percent
    void SortLowToHighPercent();
    void SortHighToLowPercent();
    // remove/re-add materials
    void RemoveAllMaterials();
    void AddSortedMaterials(QVector<Materials*>& materials);

public slots:
    // Adds a Material widget to the main window widget
    // and saves it inside a vector data structure
    void AddMaterial();
    // Removes a Material widget from the main window widget
    void RemoveMaterial(Materials* material);
    // Updates the status of in progress and completed
    // tracked materials
    void TrackingStatusChanged();
    // Search the database for any matching words
    // while the user types in the lineEdit
    void SearchBarUpdated(const QString& text);

private:
    Ui::MaterialStatusBar *ui;
    DataInterface* dataInterface;

    // --- Sorting ---
    int sortCategoryClicks;
    int sortNameClicks;
    int sortCurrentClicks;
    int sortGoalClicks;
    int sortPercentClicks;
    MaterialSorting materialSorting;

    // --- Variables: tabs ---
    QString tabName;

    // --- Data structures ---
    QVector<Materials*> trackedMaterials;
    QVector<Materials*> originalTrackedMaterials;
    QVector<Materials*> sortedTrackedMaterials;


    // --- Variables: Search material ---
    QString searchedMaterial;
    QStringList materialList;
    QCompleter* wordCompleter;

    // --- Functions: Search material ---
    void SetupWordCompleter();

public:

    // --- Inline Functions ---
    inline void UpdateAllMaterials() { for(auto material : trackedMaterials) material->UpdateUiMaterialValues(); }
};

#endif // MATERIALSTATUSBAR_H
