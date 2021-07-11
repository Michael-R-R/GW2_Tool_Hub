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

    // Enum class to keep track of all the sorting configurations
    enum class SortingState 
    { 
        DEFAULT,
        CATEGORY_NONE, CATEGORY_ASCENDING, CATEGORY_DESCENDING,
        NAME_NONE, NAME_ASCENDING, NAME_DESCENDING,
        CURRENT_NONE, CURRENT_ASCENDING, CURRENT_DESCENDING,
        GOAL_NONE, GOAL_ASCENDING, GOAL_DESCENDING,
        PERCENT_NONE, PERCENT_ASCENDING, PERCENT_DESCENDING,
    };

    // --- Functions ---
    void UpdateTrackingStatus();

    // --- Tabs ---
    QString GetTabName() { return tabName; }
    void SetTabName(const QString& val) { tabName = val; }
    void UpdateTabName(QString newTabName);

    // --- Utility ---
    int GetNumOfTrackedMaterials() { return trackedMaterials.size(); }
    QVector<QString> GetAllTrackedMaterialNames() const;
    QVector<int> GetAllTrackedCurrentAmounts() const;
    QVector<int> GetAllTrackedGoalAmounts() const;
    void AddMaterialGoalAmount(int index, int amount);
    
    // --- Adding materials ---
    void AddMaterialFromSaveFile(int amtOfMaterials,
                                 const QVector<QString>& matNames,
                                 const QVector<int>& currentAmts,
                                 const QVector<int>& goalAmts);
    void AddMaterialByCountAndName(int matCount, QString matName);

    // --- Sorting ---
    // Enum get/set sorting state
	MaterialStatusBar::SortingState GetCurrentSortState() const { return currentSortState; }
	void SetCurrentSortState(MaterialStatusBar::SortingState val) { currentSortState = val; }
	MaterialStatusBar::SortingState GetNextSortState() const { return nextSortState; }
	void SetNextSortState(MaterialStatusBar::SortingState val) { nextSortState = val; }
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
    // Sorting Utility Functions
    void RemoveAllMaterials();
    void AddSortedMaterials(QVector<Materials*>& materials);
    void MakeCopyOfTrackedMaterials();

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

    // --- Database Interface ---
    DataInterface* dataInterface;

    // --- Sorting ---
    SortingState currentSortState;
    SortingState nextSortState;
    MaterialSorting materialSorting;

    // --- Tabs ---
    QString tabName;

    // --- Data structures ---
    QVector<Materials*> trackedMaterials;
    QVector<Materials*> originalTrackedMaterials;
    QVector<Materials*> sortedTrackedMaterials;

    // --- Search Material Line Edit ---
    QString searchedMaterial;
    QStringList materialList;
    QCompleter* wordCompleter;
    void SetupWordCompleter();

public:

    // --- Inline Functions ---
    inline void UpdateAllMaterials() { for(auto material : trackedMaterials) material->UpdateUiMaterialValues(); }
};

#endif // MATERIALSTATUSBAR_H
