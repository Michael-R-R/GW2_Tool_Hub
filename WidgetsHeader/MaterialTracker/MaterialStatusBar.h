#ifndef MATERIALSTATUSBAR_H
#define MATERIALSTATUSBAR_H

#include "WidgetsHeader/MaterialTracker/Materials.h"
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
    void SetTabName(QString val) { tabName = val; }
    QString GetTabName() { return tabName; }
    void UpdateTabName(QString newTabName);

    int GetNumOfTrackedMaterials() { return trackedMaterials.size(); }
    QVector<QString> GetAllTrackedMaterialNames() const;
    QVector<int> GetAllTrackedCurrentAmounts() const;
    QVector<int> GetAllTrackedGoalAmounts() const;

    void UpdateTrackingStatus();
    void AddMaterialFromFile(int amtOfMaterials, QVector<QString> matNames,
                             QVector<int> currentAmts, QVector<int> goalAmts);


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

signals:

private:
    Ui::MaterialStatusBar *ui;
    DataInterface* dataInterface;

    // --- Variables: tabs ---
    QString tabName;

    // --- Data structures ---
    QVector<Materials*> trackedMaterials;

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
