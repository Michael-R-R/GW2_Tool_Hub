#ifndef MATERIALS_H
#define MATERIALS_H

#include "NonWidgetsHeader/DataInterface.h"
#include <QWidget>
#include <QVector>
#include <QString>
#include <QEvent>

namespace Ui { class Materials; }

class Materials : public QWidget
{
    Q_OBJECT

public:
    Materials(QWidget *parent = nullptr);
    ~Materials();

    // --- Functions ---
    QString getTabName() const { return tabName; }
    void setTabName(const QString &value) { tabName = value; }

    bool GetIsCompleted() const { return isCompleted; }
    void SetIsCompleted(const bool &val) { isCompleted = val; }

    QString GetMaterialName() const;
    int GetMaterialCurrentAmt() const;
    int GetMaterialGoalAmt() const;

    void SetMaterialValuesByName(const QString materialName);
    void SetMaterialValuesFromFile(const QString materialName, const int currentAmt, const int goalAmt);
    void UpdateUiMaterialValues();

private slots:
    // --- ComboBoxes ---
    void MaterialCatagoryChanged(QString text);
    void MaterialTypeChanged(QString text);
    // --- SpinBoxes ---
    void CurrentAmountUpdated(int amount);
    void GoalAmountUpdated(int amount);

signals:
    void RemoveMaterial(Materials* material);
    void TrackingStatusChanged(Materials* material);

private:
    // --- Pointers ---
    Ui::Materials *ui;
    DataInterface* dataInterface;

    // --- Variables ---
    QString tabName;
    bool isCompleted;

    // --- Functions ---
    void SetupAllMaterialDefaults();
    void AddMaterialCategoryTypes(const QString text); // I.E. Copper, Silver, ETC.
    void SetSpinBoxCurrentAmount(QString materialName);
    void SetSpinBoxGoalAmount(QString materialName);
    void SetPercentComplete(QString materialName);

    // --- Events ---
    bool eventFilter(QObject* object, QEvent* event);

};

#endif // MATERIALS_H
