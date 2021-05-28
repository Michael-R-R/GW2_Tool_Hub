#include "WidgetsHeader/MaterialTracker/MaterialStatusBar.h"
#include "ui_MaterialStatusBar.h"


MaterialStatusBar::MaterialStatusBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MaterialStatusBar),
    tabName(), trackedMaterials(),
    searchedMaterial(), materialList(), wordCompleter(new QCompleter(materialList, this))
{
    // Setup
    ui->setupUi(this);
    SetupWordCompleter();
    UpdateTrackingStatus();

    // Signals/Slots
    connect(ui->addMatPushButton, &QPushButton::clicked, this, &MaterialStatusBar::AddMaterial);
    connect(ui->searchMaterialLineEdit, &QLineEdit::textChanged, this, &MaterialStatusBar::SearchBarUpdated);
}

MaterialStatusBar::~MaterialStatusBar()
{
    delete ui;
    foreach(auto x, trackedMaterials) {delete x;}
    delete wordCompleter;

}


/*************************************************************************
 *                          PUBLIC FUNCTIONS                             *
 *************************************************************************/

// Loops through each material and updates its corresponding tab name
void MaterialStatusBar::UpdateTabName(QString newTabName)
{
    SetTabName(newTabName);

    for(auto material : trackedMaterials)
    {
        material->setTabName(newTabName);
    }
}

// Loops through all current trackedMaterials and retrieves the material
// name from the material type combobox
QVector<QString> MaterialStatusBar::GetAllTrackedMaterialNames() const
{
    QVector<QString> materialNames;
    for(auto material : trackedMaterials)
    {
        materialNames.append(material->GetMaterialName());
    }

    return materialNames;
}

// Loops through all current trackedMaterials and retrieves the material
// current amount from the material spinbox
QVector<int> MaterialStatusBar::GetAllTrackedCurrentAmounts() const
{
    QVector<int> materialCurrent;
    for(auto material : trackedMaterials)
    {
        materialCurrent.append(material->GetMaterialCurrentAmt());
    }

    return materialCurrent;
}

// Loops through all goal trackedMaterials and retrieves the material
// goal amount from the material spinbox
QVector<int> MaterialStatusBar::GetAllTrackedGoalAmounts() const
{
    QVector<int> materialGoal;
    for(auto material : trackedMaterials)
    {
        materialGoal.append(material->GetMaterialGoalAmt());
    }

    return materialGoal;
}

// Update the label for how many materials are being tracked
// (active / complete)
void MaterialStatusBar::UpdateTrackingStatus()
{
    double completedCount = 0;
    for(auto material : trackedMaterials)
    {
        if(material->GetIsCompleted()) { completedCount++; }
    }

    int activeAmount = trackedMaterials.size() - completedCount;
    double percentComplete = (completedCount / trackedMaterials.size()) * 100.0;
    ui->labelMatStatus->setText(QString("Status: %1 Active / %2 Complete / %3% Complete")
                                     .arg(activeAmount)
                                     .arg(completedCount)
                                     .arg(percentComplete, 2, 'g', 4));
}

// Adds all the materials that were read in from file and adds them to
// their corresponding tabs
void MaterialStatusBar::AddMaterialFromSaveFile(int amtOfMaterials, QVector<QString> matNames,
                                            QVector<int> currentAmts, QVector<int> goalAmts)
{
    for(int i = 0; i < amtOfMaterials; i++)
    {
        Materials* material = new Materials;
        material->setTabName(GetTabName());
        material->SetMaterialValuesFromSaveFile(matNames[i], currentAmts[i], goalAmts[i]);

        // Connect Material class's signals to the main window
        // These signal/slots will activate when they recieve the
        // emit signal from the Material class's functions
        connect(material, &Materials::RemoveMaterial, this, &MaterialStatusBar::RemoveMaterial);
        connect(material, &Materials::TrackingStatusChanged, this, &MaterialStatusBar::TrackingStatusChanged);

        // Add the material to the vector and
        // to the applications ui
        trackedMaterials.append(material);
        ui->matTrackerToolBarLayout->addWidget(material);

        UpdateTrackingStatus();
    }
}

// Adds all the materials that were read in from the excel file
// and adds them to the active tab
void MaterialStatusBar::AddMaterialFromExcelFile(int matCount, QString matName)
{
    Materials* material = new Materials;
    material->setTabName(GetTabName());
    // Checks if the excel item being added is an actual
    // valid material, if not, then delete this material object
    if(material->SetMaterialValuesFromExcelFile(matCount, matName))
    {
        // Connect Material class's signals to the main window
        // These signal/slots will activate when they recieve the
        // emit signal from the Material class's functions
        connect(material, &Materials::RemoveMaterial, this, &MaterialStatusBar::RemoveMaterial);
        connect(material, &Materials::TrackingStatusChanged, this, &MaterialStatusBar::TrackingStatusChanged);

        // Add the material to the vector and
        // to the applications ui
        trackedMaterials.append(material);
        ui->matTrackerToolBarLayout->addWidget(material);

        UpdateTrackingStatus();
    }
    else
    {
        delete material;
    }
}

/*************************************************************************
 *                           PUBLIC SLOTS                                *
 *************************************************************************/

// SLOT Function:
// Adds a Material widget to the MaterialTracker widget
// and saves it inside a vector data structure
// if there is a searched material in the lineEdit
// search for that material and add it
void MaterialStatusBar::AddMaterial()
{
    Materials* material = new Materials;
    material->setTabName(GetTabName());

    // Add the searched for material and clear the lineEdit
    if(!searchedMaterial.isEmpty()) { material->SetMaterialValuesByName(searchedMaterial); }
    ui->searchMaterialLineEdit->clear();
    ui->searchMaterialLineEdit->setFocus();

    // Connect Material class's signals to the main window
    // These signal/slots will activate when they recieve the
    // emit signal from the Material class's functions
    connect(material, &Materials::RemoveMaterial, this, &MaterialStatusBar::RemoveMaterial);
    connect(material, &Materials::TrackingStatusChanged, this, &MaterialStatusBar::TrackingStatusChanged);
    trackedMaterials.append(material);

    // Add the material to the applications ui
    ui->matTrackerToolBarLayout->addWidget(material);

    UpdateTrackingStatus();
}

// SLOT Function: Removes a Material widget from the main window widget
// and from the data structure
void MaterialStatusBar::RemoveMaterial(Materials* material)
{
    trackedMaterials.removeOne(material);
    ui->matTrackerToolBarLayout->removeWidget(material);
    delete material;
    UpdateTrackingStatus();
}

// SLOT Function: Updates the status of in progress and completed tracked materials
void MaterialStatusBar::TrackingStatusChanged()
{
    UpdateTrackingStatus();
}

// SLOT Function: Sets the searchMaterial varible for whatever
// text the user enters into the lineEdit
void MaterialStatusBar::SearchBarUpdated(const QString& text)
{
    searchedMaterial = text;
}

/*************************************************************************
 *                          PRIVATE FUNCTIONS                            *
 *************************************************************************/

// Sets up the word completer for the search material
// lineEdit in order to help the user find their searched material
void MaterialStatusBar::SetupWordCompleter()
{
    // Create the materialList by fetching all materials in the database
    dataInterface = new DataInterface;
    QVector<QString> materialNames;
    materialNames = dataInterface->FetchAllMaterialNames();
    for(auto x : materialNames) { materialList.append(x); }

    // Connect the world completer to the line edit
    wordCompleter = new QCompleter(materialList,this);
    wordCompleter->setFilterMode(Qt::MatchContains);
    wordCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->searchMaterialLineEdit->setCompleter(wordCompleter);

    delete dataInterface;
}







