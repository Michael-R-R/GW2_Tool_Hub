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

    QString GetMaterialCategory() const;
    QString GetMaterialName() const;
    int GetMaterialCurrentAmt() const;
    int GetMaterialGoalAmt() const;
    double GetMaterialPercentComplete() const;

    void SetMaterialValuesByName(QString materialName);
    void SetMaterialValuesFromSaveFile(QString materialName, int currentAmt, int goalAmt);
    bool SetMaterialValuesFromExcelFile(int goalAmount, QString matName);

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
    void AddMaterialCategoryTypes(QString text); // I.E. Copper, Silver, ETC.
    void SetSpinBoxCurrentAmount(QString materialName);
    void SetSpinBoxGoalAmount(QString materialName);
    void SetPercentComplete(QString materialName);

    // --- Events ---
    bool eventFilter(QObject* object, QEvent* event);

};

#endif // MATERIALS_H
