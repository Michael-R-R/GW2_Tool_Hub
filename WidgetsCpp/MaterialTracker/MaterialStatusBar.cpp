#include "WidgetsHeader/MaterialTracker/MaterialStatusBar.h"
#include "ui_MaterialStatusBar.h"


MaterialStatusBar::MaterialStatusBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MaterialStatusBar),
    currentSortState(SortingState::DEFAULT), nextSortState(SortingState::DEFAULT),
    tabName(), vTrackedMaterials(), vOriginalTrackedMaterials(), vSortedTrackedMaterials(),
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
    foreach(auto x, vTrackedMaterials) {delete x;}
    delete wordCompleter;

}


/*************************************************************************
 *                          PUBLIC FUNCTIONS                             *
 *************************************************************************/

 // Update the label for how many materials are being tracked
 // (active / complete)
void MaterialStatusBar::UpdateTrackingStatus()
{
	double completedCount = 0;
	for (auto material : vTrackedMaterials)
	{
		if (material->GetIsCompleted()) { completedCount++; }
	}

	int activeAmount = vTrackedMaterials.size() - completedCount;
	double percentComplete = (completedCount / vTrackedMaterials.size()) * 100.0;
	ui->labelMatStatus->setText(QString("Stats: %1 Active / %2 Complete / %3% Complete")
		.arg(activeAmount)
		.arg(completedCount)
		.arg(percentComplete, 2, 'g', 4));
}

// Loops through each material and updates its corresponding tab name
void MaterialStatusBar::UpdateTabName(QString newTabName)
{
    SetTabName(newTabName);

    for(auto material : vTrackedMaterials)
    {
        material->setTabName(newTabName);
    }
}

// Loops through all current trackedMaterials and retrieves the material
// name from the material type combobox
QVector<QString> MaterialStatusBar::GetAllTrackedMaterialNames() const
{
    QVector<QString> vMaterialNames;
    for(auto material : vTrackedMaterials)
    {
        vMaterialNames.append(material->GetMaterialName());
    }

    return vMaterialNames;
}

// Loops through all current trackedMaterials and retrieves the material
// current amount from the material spin box
QVector<int> MaterialStatusBar::GetAllTrackedCurrentAmounts() const
{
    QVector<int> vMaterialCurrent;
    for(auto material : vTrackedMaterials)
    {
        vMaterialCurrent.append(material->GetMaterialCurrentAmt());
    }

    return vMaterialCurrent;
}

// Loops through all goal trackedMaterials and retrieves the material
// goal amount from the material spin box
QVector<int> MaterialStatusBar::GetAllTrackedGoalAmounts() const
{
    QVector<int> vMaterialGoal;
    for(auto material : vTrackedMaterials)
    {
        vMaterialGoal.append(material->GetMaterialGoalAmt());
    }

    return vMaterialGoal;
}

void MaterialStatusBar::AddMaterialGoalAmount(int index, int amount)
{
    vTrackedMaterials[index]->SetMaterialGoalAmt(amount);
}


// Adds all the materials that were read in from file and adds them to
// their corresponding tabs
void MaterialStatusBar::AddMaterialFromSaveFile(int amtOfMaterials,
                                                const QVector<QString>& matNames,
                                                const QVector<int>& currentAmts,
                                                const QVector<int>& goalAmts)
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
        vTrackedMaterials.append(material);
        ui->matTrackerToolBarLayout->addWidget(material);
        MakeCopyOfTrackedMaterials();

        UpdateTrackingStatus();
    }
}

// Adds all the materials that were read in from the excel file
// and adds them to the active tab
void MaterialStatusBar::AddMaterialByCountAndName(int matCount, QString matName)
{
    Materials* material = new Materials;
    material->setTabName(GetTabName());
    // Checks if the excel item being added is an actual
    // valid material, if not, then delete this material object
    if(material->SetMaterialValuesByNameAndCount(matCount, matName))
    {
        // Connect Material class's signals to the main window
        // These signal/slots will activate when they recieve the
        // emit signal from the Material class's functions
        connect(material, &Materials::RemoveMaterial, this, &MaterialStatusBar::RemoveMaterial);
        connect(material, &Materials::TrackingStatusChanged, this, &MaterialStatusBar::TrackingStatusChanged);

        // Add the material to the vector and
        // to the applications ui
        vTrackedMaterials.append(material);
        ui->matTrackerToolBarLayout->addWidget(material);
        MakeCopyOfTrackedMaterials();

        UpdateTrackingStatus();
    }
    else
    {
        delete material;
    }
}

/*************************************************************************
 *                         SORTING FUNCTIONS                             *
 *************************************************************************/

// Sorts the materials back into their original order 
// before any sorting was done
void MaterialStatusBar::SortNoFilter()
{
    // Remove the materials from the UI and data structure
    RemoveAllMaterials();
    // Re-add the materials in original order
    AddSortedMaterials(vOriginalTrackedMaterials);
}

void MaterialStatusBar::SortLowToHighCategory()
{
    // Sort the materials
    vSortedTrackedMaterials = materialSorting.SortLowToHighCategory(vTrackedMaterials);
    // Remove the materials from the UI and data structure
    RemoveAllMaterials();
    // Re-add the materials in sorted order
    AddSortedMaterials(vSortedTrackedMaterials);
}

void MaterialStatusBar::SortHighToLowCategory()
{
    // Sort the materials
    vSortedTrackedMaterials = materialSorting.SortHighToLowCategory(vTrackedMaterials);
    // Remove the materials from the UI and data structure
    RemoveAllMaterials();
    // Re-add the materials in sorted order
    AddSortedMaterials(vSortedTrackedMaterials);
}

void MaterialStatusBar::SortLowToHighName()
{

    // Sort the materials
    vSortedTrackedMaterials = materialSorting.SortLowToHighName(vTrackedMaterials);
    // Remove the materials from the UI and data structure
    RemoveAllMaterials();
    // Re-add the materials in sorted order
    AddSortedMaterials(vSortedTrackedMaterials);
}

void MaterialStatusBar::SortHighToLowName()
{
    // Sort the materials
    vSortedTrackedMaterials = materialSorting.SortHighToLowName(vTrackedMaterials);
    // Remove the materials from the UI and data structure
    RemoveAllMaterials();
    // Re-add the materials in sorted order
    AddSortedMaterials(vSortedTrackedMaterials);
}

void MaterialStatusBar::SortLowToHighCurrent()
{
    // Sort the materials
    vSortedTrackedMaterials = materialSorting.SortLowToHighCurrent(vTrackedMaterials);
    // Remove the materials from the UI and data structure
    RemoveAllMaterials();
    // Re-add the materials in sorted order
    AddSortedMaterials(vSortedTrackedMaterials);
}

void MaterialStatusBar::SortHighToLowCurrent()
{
    // Sort the materials
    vSortedTrackedMaterials = materialSorting.SortHighToLowCurrent(vTrackedMaterials);
    // Remove the materials from the UI and data structure
    RemoveAllMaterials();
    // Re-add the materials in sorted order
    AddSortedMaterials(vSortedTrackedMaterials);
}

void MaterialStatusBar::SortLowToHighGoal()
{
    // Sort the materials
    vSortedTrackedMaterials = materialSorting.SortLowToHighGoal(vTrackedMaterials);
    // Remove the materials from the UI and data structure
    RemoveAllMaterials();
    // Re-add the materials in sorted order
    AddSortedMaterials(vSortedTrackedMaterials);
}

void MaterialStatusBar::SortHighToLowGoal()
{
    // Sort the materials
    vSortedTrackedMaterials = materialSorting.SortHighToLowGoal(vTrackedMaterials);
    // Remove the materials from the UI and data structure
    RemoveAllMaterials();
    // Re-add the materials in sorted order
    AddSortedMaterials(vSortedTrackedMaterials);
}

void MaterialStatusBar::SortLowToHighPercent()
{
    // Sort the materials
    vSortedTrackedMaterials = materialSorting.SortLowToHighPercent(vTrackedMaterials);
    // Remove the materials from the UI and data structure
    RemoveAllMaterials();
    // Re-add the materials in sorted order
    AddSortedMaterials(vSortedTrackedMaterials);
}

void MaterialStatusBar::SortHighToLowPercent()
{
    // Sort the materials
    vSortedTrackedMaterials = materialSorting.SortHighToLowPercent(vTrackedMaterials);
    // Remove the materials from the UI and data structure
    RemoveAllMaterials();
    // Re-add the materials in sorted order
    AddSortedMaterials(vSortedTrackedMaterials);
}

// Removes all the materials from the UI display and data structure
void MaterialStatusBar::RemoveAllMaterials()
{
    // Remove all the materials from the last to first
    while(vTrackedMaterials.size() > 0)
    {
        ui->matTrackerToolBarLayout->removeWidget(vTrackedMaterials.last());
        vTrackedMaterials.removeOne(vTrackedMaterials.last());
        UpdateTrackingStatus();
    }
}

// Re-adds all the now sorted materials back to the UI display and
// data structure
void MaterialStatusBar::AddSortedMaterials(QVector<Materials*>& materials)
{
    for(int i = 0; i < materials.size(); i++)
    {
        materials[i]->setTabName(GetTabName());

        // Connect Material class's signals to the main window
        // These signal/slots will activate when they receive the
        // emit signal from the Material class's functions
        connect(materials[i], &Materials::RemoveMaterial, this, &MaterialStatusBar::RemoveMaterial);
        connect(materials[i], &Materials::TrackingStatusChanged, this, &MaterialStatusBar::TrackingStatusChanged);

        vTrackedMaterials.append(materials[i]);
        ui->matTrackerToolBarLayout->addWidget(materials[i]);

        UpdateTrackingStatus();
    }
}

// Makes a copy of the original material order in case the user 
// wants to display the materials in unsorted order again
void MaterialStatusBar::MakeCopyOfTrackedMaterials()
{
    vOriginalTrackedMaterials = vTrackedMaterials;
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

    // Add material to the vector and to the UI display
    vTrackedMaterials.append(material);
    ui->matTrackerToolBarLayout->addWidget(material);
    MakeCopyOfTrackedMaterials();

    UpdateTrackingStatus();
}

// SLOT Function: Removes a Material widget from the main window widget
// and from the data structure
void MaterialStatusBar::RemoveMaterial(Materials* material)
{
    vTrackedMaterials.removeOne(material);
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
    QVector<QString> vNames;
    vNames = dataInterface->FetchAllMaterialNames();
    for(auto material : vNames) { materialList.append(material); }

    // Connect the word completer to the line edit
    wordCompleter = new QCompleter(materialList, this);
    wordCompleter->setFilterMode(Qt::MatchContains);
    wordCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->searchMaterialLineEdit->setCompleter(wordCompleter);

    delete dataInterface;
}







