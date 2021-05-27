#include "WidgetsHeader/MaterialTracker/Materials.h"
#include "ui_Materials.h"
#include <QDebug>
#include <QStyleOption>

Materials::Materials(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Materials),
    dataInterface(new DataInterface),
    tabName(), isCompleted(false)
{
    // Setup
    ui->setupUi(this);
    SetupAllMaterialDefaults();

    // Event Filter: disables combobox and spinbox scrolling
    // unless selected and focused on
    ui->goalAmountSpinBox->installEventFilter(this);
    ui->materialCatagoryComboBox->installEventFilter(this);
    ui->materialTypesComboBox->installEventFilter(this);

    // Signals/Slots: ComboBoxes
    connect(ui->materialCatagoryComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(MaterialCatagoryChanged(QString)));
    connect(ui->materialTypesComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(MaterialTypeChanged(QString)));
    // Signals/Slots: SpinBoxes
    connect(ui->currentAmountSpinBox, SIGNAL(valueChanged(int)), this, SLOT(CurrentAmountUpdated(int)));
    connect(ui->goalAmountSpinBox, SIGNAL(valueChanged(int)), this, SLOT(GoalAmountUpdated(int)));

    // Signal lambdas
    connect(ui->removePushButton, &QPushButton::clicked, [this] {emit RemoveMaterial(this);});
}

Materials::~Materials()
{
    delete ui;
    delete dataInterface;
}

/*************************************************************************
 *                          PUBLIC FUNCTIONS                             *
 *************************************************************************/

// Returns the name of the current material in the
// material type combobox
QString Materials::GetMaterialName() const
{
    return ui->materialTypesComboBox->currentText();
}

// Returns the current amount of the current
// material in the material spinbox
int Materials::GetMaterialCurrentAmt() const
{
    return ui->currentAmountSpinBox->value();
}

// Returns the goal amount of the current
// material in the material spinbox
int Materials::GetMaterialGoalAmt() const
{
    return ui->goalAmountSpinBox->value();
}

// Queries the database for the passed in material
// and sets comboboxes data values
void Materials::SetMaterialValuesByName(const QString materialName)
{
    QString category = dataInterface->FetchMaterialCategory(materialName);
    ui->materialCatagoryComboBox->setCurrentText(category);
    ui->materialTypesComboBox->setCurrentText(materialName);
}

// Takes all the information read in from file and sets all the values
// for each material
void Materials::SetMaterialValuesFromFile(const QString materialName, const int currentAmt, const int goalAmt)
{
    QString category = dataInterface->FetchMaterialCategory(materialName);
    ui->materialCatagoryComboBox->setCurrentText(category);
    ui->materialTypesComboBox->setCurrentText(materialName);
    ui->currentAmountSpinBox->setValue(currentAmt);
    ui->goalAmountSpinBox->setValue(goalAmt);
}

// Updates ui values for the materials
// current amount, goal amount, and percents
void Materials::UpdateUiMaterialValues()
{
    QString currentMaterial = ui->materialTypesComboBox->currentText();
    SetSpinBoxCurrentAmount(currentMaterial);
    SetSpinBoxGoalAmount(currentMaterial);
    SetPercentComplete(currentMaterial);
}

/*************************************************************************
 *                          PRIVATE SLOTS                                *
 *************************************************************************/

// SLOT Function: called when the material catagory combobox value is changed
void Materials::MaterialCatagoryChanged(const QString text)
{
    // Adds all the corresponding material types (material names) with that category
    AddMaterialCategoryTypes(text);
}

// SLOT Function: called when the material type combobox is changed,
// Changes the spinbox values to the correct material type
void Materials::MaterialTypeChanged(const QString text)
{
    // Fetches and sets the spinboxes and percent label for
    // the selected material
    dataInterface->UpdatePercentComplete(text, tabName);
    SetSpinBoxCurrentAmount(text);
    SetSpinBoxGoalAmount(text);
    SetPercentComplete(text);
}

// SLOT Function: called when the value is changed inside the spinbox
// Saves the currentAmount value for the corresponding material type in the database
void Materials::CurrentAmountUpdated(int amount)
{
    QString currentMaterial = ui->materialTypesComboBox->currentText();
    dataInterface->UpdateCurrentMaterialsAmount(currentMaterial, amount, tabName);
    dataInterface->UpdatePercentComplete(currentMaterial, tabName);
    SetPercentComplete(currentMaterial);
}

// SLOT Function: called when the value is changed inside the spinbox
// Saves the goalAmount value for the corresponding material type in the database
void Materials::GoalAmountUpdated(int amount)
{
    QString currentMaterial = ui->materialTypesComboBox->currentText();
    dataInterface->UpdateGoalMaterialsAmount(currentMaterial, amount, tabName);
    dataInterface->UpdatePercentComplete(currentMaterial, tabName);
    SetPercentComplete(currentMaterial);
}

/*************************************************************************
 *                        PRIVATE FUNCTIONS                              *
 *************************************************************************/

// Adds all the categories and material names to the comboboxes,
// Sets the corresponding material current amount, goal amount, and
// percent complete values taken from the database
void Materials::SetupAllMaterialDefaults()
{
    // Fetch all the differenet material catagories and add them to
    // the combobox
    QVector<QString> categories = dataInterface->FetchAllMaterialCategories();
    for(auto item : categories)
    {
        ui->materialCatagoryComboBox->addItem(item);
    }

    // Add all the material names of the passed in category name
    AddMaterialCategoryTypes(ui->materialCatagoryComboBox->currentText());

    // Set the spinboxes and percent values
    QString currentMaterialName = ui->materialTypesComboBox->currentText();
    SetSpinBoxCurrentAmount(currentMaterialName);
    SetSpinBoxGoalAmount(currentMaterialName);
    SetPercentComplete(currentMaterialName);
}

// Adds all the material names to the combobox
// for the current selected category combobox
void Materials::AddMaterialCategoryTypes(const QString category)
{
    ui->materialTypesComboBox->clear();

    QVector<QString> types = dataInterface->FetchAllMaterialNamesByCategory(category);
    for(auto item : types)
    {
        ui->materialTypesComboBox->addItem(item);
    }
}

// Fetches the current amount from the database for the selected
// material and sets the spinbox value
void Materials::SetSpinBoxCurrentAmount(QString materialName)
{
    int currentAmount = dataInterface->FetchCurrentMaterialAmount(materialName, tabName);
    ui->currentAmountSpinBox->setValue(currentAmount);
}

// Fetches the goal amount from the database for the selected
// material and sets the spinbox value
void Materials::SetSpinBoxGoalAmount(QString materialName)
{
    int goalAmount = dataInterface->FetchGoalMaterialAmount(materialName, tabName);
    ui->goalAmountSpinBox->setValue(goalAmount);
}

// Called whenever current or goal amount change
// Sets the overall percent till completion and emits TrackingStatusChanged signal
// Emits: TrackingStatusChanged
void Materials::SetPercentComplete(QString materialName)
{
    double percentComplete = dataInterface->FetchPercentComplete(materialName, tabName);

    ui->percentCompleteLabel->setText(QString("%1%").arg(QString::number(percentComplete, 'g', 2)));

    // Set the color for the percentage label
    // green = 100%, 50%-99.9% = orange, 0%-49.9% = red
    if(percentComplete >= 100.0)
    {
        // Green
        ui->percentCompleteLabel->setStyleSheet(QString("background-color: rgb(100, 255, 98);"
                                                        "border: 1px solid black;"));
        SetIsCompleted(true);
    }
    else if(percentComplete >= 50.0)
    {
        // Orange
        ui->percentCompleteLabel->setStyleSheet(QString("background-color: rgb(255, 143, 63);"
                                                        "border: 1px solid black;"));
        SetIsCompleted(false);
    }
    else
    {
        // Red
        ui->percentCompleteLabel->setStyleSheet(QString("background-color: rgb(247, 20, 0);"
                                                        "border: 1px solid black;"));
        SetIsCompleted(false);
    }

    // Emit signal whenever the percentage changes
    emit TrackingStatusChanged(this);
}

/*************************************************************************
 *                          EVENT FILTERS                                *
 *************************************************************************/

// Disables combobox and spinbox contents scrolling
// unless selected and focused on
bool Materials::eventFilter(QObject *object, QEvent *event)
{
    // SPIN BOX
    QAbstractSpinBox* spinBox = qobject_cast<QAbstractSpinBox*>(object);
    if(spinBox)
    {
        if(event->type() == QEvent::Wheel)
        {
            if(spinBox->focusPolicy() == Qt::WheelFocus)
            {
                event->accept();
                return false;
            }
            else
            {
                event->ignore();
                return true;
            }
        }
        else if(event->type() == QEvent::FocusIn)
        {
            spinBox->setFocusPolicy(Qt::WheelFocus);
        }
        else if(event->type() == QEvent::FocusOut)
        {
            spinBox->setFocusPolicy(Qt::StrongFocus);
        }
    }

    // COMBO BOX
    QComboBox* comboBox = qobject_cast<QComboBox*>(object);
    if(comboBox)
    {
        if(event->type() == QEvent::Wheel)
        {
            if(comboBox->focusPolicy() == Qt::WheelFocus)
            {
                event->accept();
                return false;
            }
            else
            {
                event->ignore();
                return true;
            }
        }
        else if(event->type() == QEvent::FocusIn)
        {
            comboBox->setFocusPolicy(Qt::WheelFocus);
        }
        else if(event->type() == QEvent::FocusOut)
        {
            comboBox->setFocusPolicy(Qt::StrongFocus);
        }
    }

    return QObject::eventFilter(object, event);
}









