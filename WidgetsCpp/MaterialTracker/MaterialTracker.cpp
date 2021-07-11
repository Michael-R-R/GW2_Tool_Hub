#include "WidgetsHeader/MaterialTracker/MaterialTracker.h"
#include "ui_MaterialTracker.h"

MaterialTracker::MaterialTracker(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MaterialTracker), saveAndLoad(new SaveAndLoad),
    materialTabs(), tabName(),
    resultName(), resultCount(),
    searchedRecipe(), recipeList(), wordCompleter(nullptr)
{
    // Setup
    ui->setupUi(this);
    SetIcons();
    SetupRecipeWordCompleter();

    // Signals/Slots
    // Materials
    connect(ui->addTabPushButton, &QPushButton::clicked, this, &MaterialTracker::AddTab);
    connect(ui->materialsTabWidget->tabBar(), SIGNAL(tabCloseRequested(int)), this, SLOT(RemoveTab(int)));
	connect(ui->materialsTabWidget->tabBar(), SIGNAL(tabBarClicked(int)), this, SLOT(SortingConfig(int)));
    connect(ui->materialsTabWidget->tabBar(), SIGNAL(tabBarDoubleClicked(int)), this, SLOT(RenameTab(int)));
    connect(ui->updatePushButton, &QPushButton::clicked, this, &MaterialTracker::UpdateMaterials);
    // Save n Load
    connect(ui->savePushButton, &QPushButton::clicked, this, &MaterialTracker::SaveFile);
    connect(ui->loadPushButton, &QPushButton::clicked, this, &MaterialTracker::OpenFile);
    // Sorting
    connect(ui->sortCategoryPushButton, &QPushButton::clicked, this, &MaterialTracker::SortByCategory);
    connect(ui->sortNamePushButton, &QPushButton::clicked, this, &MaterialTracker::SortByName);
    connect(ui->sortCurrentPushButton, &QPushButton::clicked, this, &MaterialTracker::SortByCurrent);
    connect(ui->sortGoalPushButton, &QPushButton::clicked, this, &MaterialTracker::SortByGoal);
    connect(ui->sortPercentPushButton, &QPushButton::clicked, this, &MaterialTracker::SortByPercent);
}

MaterialTracker::~MaterialTracker()
{
	delete ui;
    delete saveAndLoad;
	DeleteAllTabDataTables();
	foreach(auto x, materialTabs) { delete x; }
    delete wordCompleter;
}

/*************************************************************************
 *                           PUBLIC FUNCTIONS                            *
 *************************************************************************/

void MaterialTracker::ChangeWaitingForApiReplyStatusLabel()
{
    // Keep track of the amount of dots to add to the status label
    static int numOfDots = 1;
    switch (numOfDots)
    {
    case 1:
        ui->fileStatusLabel->setText(QString("Loading API Reply."));
        numOfDots++;
        break;

    case 2:
        ui->fileStatusLabel->setText(QString("Loading API Reply.."));
        numOfDots++;
        break;

    case 3:
        ui->fileStatusLabel->setText(QString("Loading API Reply..."));
        numOfDots = 1;
        break;
    }
}

QVector<QString> MaterialTracker::GetAllTabNames()
{
    QVector<QString> tabNames;
    for (auto material : materialTabs)
    {
        tabNames.append(material->GetTabName());
    }

    return tabNames;
}

void MaterialTracker::SearchRecipe()
{
    QInputDialog *dialog = new QInputDialog(this);
    dialog->setWindowTitle("Search Recipe");
    dialog->setLabelText("Recipe Name");
    dialog->setTextValue("");
    QLineEdit *lineEdit = dialog->findChild<QLineEdit*>();
    lineEdit->setCompleter(wordCompleter);
    dialog->exec();

    searchedRecipe = dialog->textValue();
    RecursiveSearchRecipe(searchedRecipe);
    for (int i = 0; i < resultName.size(); i++)
    {
        QString name = resultName[i];
        int count = resultCount[i];
        materialTabs[ui->materialsTabWidget->currentIndex()]->AddMaterialByCountAndName(count, name);
    }
    resultName.clear();
    resultCount.clear();
}

void MaterialTracker::RecursiveSearchRecipe(QString recipeName)
{
    QApplication::processEvents();

    // Query RecipesCatalog database with the recipe name
    DataInterface dataInterface;
    QVector<QString> ingredientNames = dataInterface.FetchIngredientsNames(recipeName);
    QVector<QString> ingredientCounts = dataInterface.FetchIngredientsCount(recipeName);

    // Check if ingredient names are a recipe
    bool isRecipeValid = false;
    foreach (QString item, ingredientNames)
    {
        // Check if there are any recipes that need further processing
        isRecipeValid = dataInterface.CheckForValidRecipe(item);
        if (isRecipeValid)
        {
            QString tempName = item;
            int index = ingredientNames.indexOf(tempName);
            int tempCount = ingredientCounts[index].toInt();
            ingredientNames.removeAt(index);
            ingredientCounts.removeAt(index);
            for (int i = 0; i < tempCount; i++)
            {
                RecursiveSearchRecipe(tempName);
            }
        }
        else
        {
            QString tempName = item;
            int index = ingredientNames.indexOf(tempName);
            int tempCount = ingredientCounts[index].toInt();

            // Check if the material is already in the result vector
            if (resultName.contains(tempName))
            {
                int index = resultName.indexOf(tempName);
                resultCount[index] += tempCount;
            }
            else
            {
                resultName.append(tempName);
                resultCount.append(tempCount);
            }
        }
    }
}

void MaterialTracker::ImportExcelSheet()
{
    // Throw an error if there are no active tabs
    if (materialTabs.size() <= 0)
    {
        error.NonModalErrorMessage(this, "Error", "No Active Tabs");
        return;
    }

    // Set status label
    ui->fileStatusLabel->setText("Importing File...");

    // Allow user to select the .csv excel file
    SaveAndLoad* loadExcel = new SaveAndLoad;
    QByteArray inFile = loadExcel->LoadExcelSheet(this);
    delete loadExcel;

    // Check if invalid file loaded and update status label
    if (inFile == "invalid")
    {
        ui->fileStatusLabel->setText("Importing File Unsuccessful");
        return;
    }

    // FILE IS VALID
    // Parse the count and material name
    QVector<int> matCounts;
    QVector<QString> matNames;
    bool isFirstLine = true;

    // Parse each line for count info and material name
    QTextStream fileResult(&inFile);
    while (!fileResult.atEnd())
    {
        // Don't read the first line
        QString line = fileResult.readLine();
        if (!isFirstLine)
        {
            // PARSE THE COUNT INFO
            // Remove the first comma
            int tempIndex = line.indexOf(",");
            line.remove(tempIndex, 1);

            // Get the index of the second comma
            // Tells us when to stop reading the count info
            tempIndex = line.indexOf(",");
            QString tempCount;
            for (int i = 0; i < tempIndex; i++)
            {
                // Reads up until the comma = material count
                tempCount += line[i];
            }
            // Remove the count info from the line
            for (int i = 0; i < tempCount.size(); i++)
            {
                line.remove(0, 1);
            }
            // Delete the left over comma
            tempIndex = line.indexOf(",");
            line.remove(tempIndex, 1);
            // Add the count to the vector
            matCounts.append(tempCount.toInt());

            // PARSE THE MATERIAL NAME
            tempIndex = line.indexOf(",");
            QString tempName;
            for (int i = 0; i < tempIndex; i++)
            {
                // Reads up until the comma = material name
                tempName += line[i];
            }
            // add the name to the vector
            matNames.append(tempName);
        }
        else
        {
            // First line is junk, do nothing
            isFirstLine = false;
        }
    }

    // Add the material counts and names to the active tab
    int currentActiveTab = ui->materialsTabWidget->currentIndex();
    for (int i = 0; i < matCounts.size(); i++)
    {
        materialTabs[currentActiveTab]->AddMaterialByCountAndName(matCounts[i], matNames[i]);
    }

    ui->fileStatusLabel->setText("Importing File Complete");
}

/*************************************************************************
 *                            PUBLIC SLOTS                               *
 *************************************************************************/

// SLOT Function:
// Writes out all the current tabs and each material being tracked
// to file
void MaterialTracker::SaveFile()
{
    ui->fileStatusLabel->setText("Saving File...");

    // GET AMT OF TABS
    int tabAmt = materialTabs.size();

    // GET AND STORE THE ALL INFORMATION ABOUT EACH TAB
    QVector<QString> vTabNames;
    QVector<int> vAmtOfMatInTab;
    QVector<QVector<QString>> vNamesInTab;
    QVector<QVector<int>> vCurrentAmtsInTab;
    QVector<QVector<int>> vGoalAmtsInTab;
    for(auto tab : materialTabs)
    {
        // GET EACH TAB NAME
        vTabNames.append(tab->GetTabName());

        // GET AMOUNT OF MATERIALS IN TAB
        vAmtOfMatInTab.append(tab->GetNumOfTrackedMaterials());

        // GET MATERIAL NAMES IN TAB
        vNamesInTab.append(tab->GetAllTrackedMaterialNames());

        // GET MATERIAL CURRENT AMT IN TAB
        vCurrentAmtsInTab.append(tab->GetAllTrackedCurrentAmounts());

        // GET MATERIAL GOAL AMT IN TAB
        vGoalAmtsInTab.append(tab->GetAllTrackedGoalAmounts());
    }

    // Pass all the gathered data to the save function
    // to be written out to file
    int successResult = saveAndLoad->SaveTrackedMaterials(this, tabAmt, vTabNames, vAmtOfMatInTab,
                                                       vNamesInTab, vCurrentAmtsInTab, vGoalAmtsInTab);
    // Updates the status label
    // -1 = unsuccesful, 0 = cancelled, 1 = successful
    CheckSaveFileStatus(successResult);
}

void MaterialTracker::SaveAsFile()
{
    ui->fileStatusLabel->setText("Saving As File...");

    // GET AMT OF TABS
    int tabAmt = materialTabs.size();

    // GET AND STORE THE ALL INFORMATION ABOUT EACH TAB
    QVector<QString> vTabNames;
    QVector<int> vAmtOfMatInTab;
    QVector<QVector<QString>> vNamesInTab;
    QVector<QVector<int>> vCurrentAmtsInTab;
    QVector<QVector<int>> vGoalAmtsInTab;
    for (auto tab : materialTabs)
    {
        // GET EACH TAB NAME
        vTabNames.append(tab->GetTabName());

        // GET AMOUNT OF MATERIALS IN TAB
        vAmtOfMatInTab.append(tab->GetNumOfTrackedMaterials());

        // GET MATERIAL NAMES IN TAB
        vNamesInTab.append(tab->GetAllTrackedMaterialNames());

        // GET MATERIAL CURRENT AMT IN TAB
        vCurrentAmtsInTab.append(tab->GetAllTrackedCurrentAmounts());

        // GET MATERIAL GOAL AMT IN TAB
        vGoalAmtsInTab.append(tab->GetAllTrackedGoalAmounts());
    }

    // Pass all the gathered data to the save function
    // to be written out to file
    int successResult = saveAndLoad->SaveAsTrackedMaterials(this, tabAmt, vTabNames, vAmtOfMatInTab,
                                                       vNamesInTab, vCurrentAmtsInTab, vGoalAmtsInTab);
    // Updates the status label
    // -1 = unsuccesful, 0 = cancelled, 1 = successful
    CheckSaveFileStatus(successResult);
}

// SLOT Function:
// Reads in all the current tabs and each material being tracked
// to file
void MaterialTracker::OpenFile()
{
    ui->fileStatusLabel->setText("Loading File...");

    // Loads in the file and retrieves the returned
    // QByteArray data and then adds that data
    // to a QTextStream to be easily parsed through
    // line by line
    QByteArray inFile = saveAndLoad->OpenTrackedMaterialsFile(this);
    QTextStream fileResult(&inFile);

    // Check if there isn't anything to load
    if(inFile == "invalid")
    {
        ui->fileStatusLabel->setText("Loading Canceled");
        return;
    }

    // Temp variables
    int tempAmtOfMatInTab = 0;
    // GET AND STORE ALL THE INFORMATION ABOUT EACH TAB
    int tabAmt = 0;
    QVector<QString> vTabNames;
    QVector<int> vAmtOfMatInTab;
    QVector<QVector<QString>> vNamesInTab;
    QVector<QVector<int>> vCurrentAmtsInTab;
    QVector<QVector<int>> vGoalAmtsInTab;
    while(!fileResult.atEnd())
    {
        // Process other events while loading files
        // and update status bar to show user the files are loading
        QCoreApplication::processEvents();
        ChangeLoadingFileStatusLabel();

        // READ ONE LINE AT A TIME
        QString line = fileResult.readLine();

        // GET AMT OF TABS
        if(line.contains("Amount of Tabs:"))
        {
            QString temp;
            for(int i = line.indexOf(":") + 2; i < line.size(); i++)
            {
                temp += line[i];
            }
            tabAmt = temp.toInt();
        }

        // GET TAB NAME
        else if(line.contains("Tab Name:"))
        {
            QString temp;
            for(int i = line.indexOf(":") + 2; i < line.size(); i++)
            {
                temp += line[i];
            }
            vTabNames.append(temp);
        }

        // GET AMT OF MATERIALS IN TAB
        else if(line.contains("Amount of Materials:"))
        {
            QString temp;
            for(int i = line.indexOf(":") + 2; i < line.size(); i++)
            {
                temp += line[i];
            }
            tempAmtOfMatInTab = temp.toInt();
            vAmtOfMatInTab.append(temp.toInt());
        }

        // GET ALL MATERIALS NAMES, CURRENT AMT, AND GOAL AMT IN TAB
        else if(line.contains("Material Name:"))
        {
            QVector<QString> tempMatNames;
            QVector<int> tempCurrentAmt;
            QVector<int> tempGoalAmt;
            for(int i = 0; i < tempAmtOfMatInTab; i++)
            {
                if(line.contains("Material Name:"))
                {
                    QString temp;
                    for(int i = line.indexOf(":") + 2; i < line.size(); i++)
                    {
                        temp += line[i];
                    }
                    tempMatNames.append(temp);
                    // Read next line
                    line = fileResult.readLine();
                }
                if(line.contains("Material Current Amt:"))
                {
                    QString temp;
                    for(int i = line.indexOf(":") + 2; i < line.size(); i++)
                    {
                        temp += line[i];
                    }
                    tempCurrentAmt.append(temp.toInt());
                    // Read next line
                    line = fileResult.readLine();
                }
                if(line.contains("Material Goal Amt:"))
                {
                    QString temp;
                    for(int i = line.indexOf(":") + 2; i < line.size(); i++)
                    {
                        temp += line[i];
                    }
                    tempGoalAmt.append(temp.toInt());
                }

                // Read next line, unless this is the last iteration
                if(i != tempAmtOfMatInTab - 1)
                {
                    line = fileResult.readLine();
                }
            }
            // Append all at same time to keep the indexing
            // for each materials data the same and in order
            vNamesInTab.append(tempMatNames);
            vCurrentAmtsInTab.append(tempCurrentAmt);
            vGoalAmtsInTab.append(tempGoalAmt);
        }
    }

    // CREATE THE TABS WITH THE INFORMATION
    CreateTabsFromFile(tabAmt, vTabNames, vAmtOfMatInTab,
                       vNamesInTab, vCurrentAmtsInTab,
                       vGoalAmtsInTab);

    ui->fileStatusLabel->setText("Loading Complete");
}

/*************************************************************************
 *                         PRIVATE SLOTS                                 *
 *************************************************************************/

// SLOT Function:
// Triggers whenever the user hits the 'Add Tab' push button
void MaterialTracker::AddTab()
{
    // Pop up dialog for user to enter new Tab name
    bool isOk;
    QString name = QInputDialog::getText(this, tr("Enter tab name"),
                                         tr("Tab Name"), QLineEdit::Normal,
                                         tr("New Tab"), &isOk);

    // Check if the tab name is valid
    // Not valid name, return
    if(!error.CheckForValidTabName(this, name, GetAllTabNames()))
    {
        return;
    }
    // Is a valid name, add the new tab
    else if(isOk && !name.isEmpty())
    {
        // Create the data table for the new tab
        CreateTabDataTable(name);

        // Creates a new MaterialStatusBar widget,
        // adds it to the data structure, and
        // creates a new tab with the widget
        MaterialStatusBar* tab = new MaterialStatusBar;
        tab->SetTabName(name); // Set the tab name for that stores this widget
        materialTabs.append(tab);
        ui->materialsTabWidget->addTab(tab, name);
    }
}

// SLOT Function:
// Triggers whenever the user hits the 'X' icon on the tab
void MaterialTracker::RemoveTab(int index)
{
    // Reset the sorting buttons to default text
    ResetSortingButtons();
    
    // Remove the data table from the database and then remove
    // the tab bar from the UI and vector
    DeleteTabDataTable(ui->materialsTabWidget->tabText(index));
    ui->materialsTabWidget->removeTab(index);
    materialTabs.remove(index);

	// If there are still active tabs, reset the sorting
	// config for the current tab to resume the sorting button
	// config
	if (materialTabs.size() > 0)
	{
		SortingConfig(ui->materialsTabWidget->currentIndex());
	}
}

// SLOT Function:
// Triggers whenever the user double clicks the tab
void MaterialTracker::RenameTab(int index)
{
    // Pop up dialog for user to enter new Tab name
    bool isOk;
    QString name = QInputDialog::getText(this, tr("Enter tab name"),
                                            tr("Tab Name"), QLineEdit::Normal,
                                            tr(""), &isOk);
    // Check if the new tab name is valid
    // Not valid name, return
    if(!error.CheckForValidTabName(this, name, GetAllTabNames()))
    {
        return;
    }
    // Is a valid name, rename the tab
    else if(isOk && !name.isEmpty())
    {
        // Update the database with the new tab name
        QString currentTabName = materialTabs[index]->GetTabName();
        DataInterface* dataInterface = new DataInterface;
        dataInterface->UpdateTabDataTableName(currentTabName, name);
        delete dataInterface;

        // Update the ui with the new tab name and
        // update the materials in that tab for the new
        // tab name
        ui->materialsTabWidget->tabBar()->setTabText(index, name);
        materialTabs[index]->UpdateTabName(name);
    }
}

// Loops through each tab and all materials in that tab
// and updates their values retrieved from the API network
void MaterialTracker::UpdateMaterials()
{
	// Throw an error if there are no active tabs
	if (materialTabs.size() <= 0)
	{
		error.NonModalErrorMessage(this, "Error", "No Active Tabs");
		return;
	}

    // Query the network
    ApiAccess* api = new ApiAccess;
    api->QueryForMaterialsAPI(this);

    // After retrieving the API data
    // update all the values for the ui
    for(auto tab : materialTabs)
    {
        tab->UpdateAllMaterials();
        // Show that we are waiting for the
        // API reply
        ChangeMaterialUpdatingStatusLabel();
    }

    ui->fileStatusLabel->setText(QString("Materials Updated"));
}

// Function to keep track of all the tabs and their 
// sorting button configurations
void MaterialTracker::SortingConfig(int index)
{
    ResetSortingButtons();

	// Update the tab's sorting buttons to their remembered config
	switch (materialTabs[index]->GetCurrentSortState())
	{
	case MaterialStatusBar::SortingState::CATEGORY_NONE:
		// No filter
		ui->sortCategoryPushButton->setText("Sort Category");
		break;

	case MaterialStatusBar::SortingState::CATEGORY_ASCENDING:
		// Lowest-Highest
		ui->sortCategoryPushButton->setText("Sort Category - Ascending");
		break;

	case MaterialStatusBar::SortingState::CATEGORY_DESCENDING:
		// Highest-Lowest
		ui->sortCategoryPushButton->setText("Sort Category - Descending");
		break;

	case MaterialStatusBar::SortingState::NAME_NONE:
		// No filter
		ui->sortNamePushButton->setText("Sort Name");
		break;

	case MaterialStatusBar::SortingState::NAME_ASCENDING:
		// Lowest-Highest
		ui->sortNamePushButton->setText("Sort Name - Ascending");
		break;

	case MaterialStatusBar::SortingState::NAME_DESCENDING:
		// Highest-Lowest
		ui->sortNamePushButton->setText("Sort Name - Descending");
		break;

	case MaterialStatusBar::SortingState::CURRENT_NONE:
		// No filter
		ui->sortCurrentPushButton->setText("Sort Current");
		break;

	case MaterialStatusBar::SortingState::CURRENT_ASCENDING:
		// Lowest-Highest
		ui->sortCurrentPushButton->setText("Sort Current - Ascending");
		break;

	case MaterialStatusBar::SortingState::CURRENT_DESCENDING:
		// Highest-Lowest
		ui->sortCurrentPushButton->setText("Sort Current - Descending");
		break;

	case MaterialStatusBar::SortingState::GOAL_NONE:
		// No filter
		ui->sortGoalPushButton->setText("Sort Goal");
		break;

	case MaterialStatusBar::SortingState::GOAL_ASCENDING:
		// Lowest-Highest
		ui->sortGoalPushButton->setText("Sort Goal - Ascending");
		break;

	case MaterialStatusBar::SortingState::GOAL_DESCENDING:
		// Highest-Lowest
		ui->sortGoalPushButton->setText("Sort Goal - Descending");
		break;

	case MaterialStatusBar::SortingState::PERCENT_NONE:
		// No filter
		ui->sortPercentPushButton->setText("Sort Percent");
		break;

	case MaterialStatusBar::SortingState::PERCENT_ASCENDING:
		// Lowest-Highest Percentage
		ui->sortPercentPushButton->setText("Sort Percent - Ascending");
		break;

	case MaterialStatusBar::SortingState::PERCENT_DESCENDING:
		// Highest-Lowest Percentage
		ui->sortPercentPushButton->setText("Sort Percent - Descending");
		break;

	default:
		ui->sortCategoryPushButton->setText("Sort Category");
        ui->sortNamePushButton->setText("Sort Name");
        ui->sortCurrentPushButton->setText("Sort Current");
        ui->sortGoalPushButton->setText("Sort Goal");
        ui->sortPercentPushButton->setText("Sort Percent");
		break;
	}
}

// Function that sorts all the materials in the tab by category
// 3 possible sorts: No sort; Ascending; Descending
void MaterialTracker::SortByCategory()
{
    // Throw an error if there are no active tabs
    if(materialTabs.size() <= 0)
    {
        error.NonModalErrorMessage(this, "Error", "No Active Tabs");
        return;
    }

    int currentTab = ui->materialsTabWidget->currentIndex();

    // Retrieve the next sort state when the sorting button is pressed
    // Once retrieved run the sorting algorithm and set the current sort state
    // and the next sort state for when the button is pressed again
 	switch (materialTabs[currentTab]->GetNextSortState())
	{
    case MaterialStatusBar::SortingState::CATEGORY_NONE:
		// No filter
		materialTabs[currentTab]->SortNoFilter();
		ui->sortCategoryPushButton->setText("Sort Category");
        materialTabs[currentTab]->SetCurrentSortState(MaterialStatusBar::SortingState::CATEGORY_NONE);
		materialTabs[currentTab]->SetNextSortState(MaterialStatusBar::SortingState::CURRENT_ASCENDING);
		break;

	case MaterialStatusBar::SortingState::CATEGORY_ASCENDING:
		// Lowest-Highest
		materialTabs[currentTab]->SortLowToHighCategory();
		ui->sortCategoryPushButton->setText("Sort Category - Ascending");
        materialTabs[currentTab]->SetCurrentSortState(MaterialStatusBar::SortingState::CATEGORY_ASCENDING);
		materialTabs[currentTab]->SetNextSortState(MaterialStatusBar::SortingState::CATEGORY_DESCENDING);
		break;

	case MaterialStatusBar::SortingState::CATEGORY_DESCENDING:
		// Highest-Lowest
		materialTabs[currentTab]->SortHighToLowCategory();
		ui->sortCategoryPushButton->setText("Sort Category - Descending");
        materialTabs[currentTab]->SetCurrentSortState(MaterialStatusBar::SortingState::CATEGORY_DESCENDING);
		materialTabs[currentTab]->SetNextSortState(MaterialStatusBar::SortingState::CATEGORY_NONE);
		break;

        // Default when the button hasn't been pressed yet
    default:
		// Lowest-Highest
        ResetSortingButtons();
		materialTabs[currentTab]->SortLowToHighCategory();
		ui->sortCategoryPushButton->setText("Sort Category - Ascending");
        materialTabs[currentTab]->SetCurrentSortState(MaterialStatusBar::SortingState::CATEGORY_ASCENDING);
		materialTabs[currentTab]->SetNextSortState(MaterialStatusBar::SortingState::CATEGORY_DESCENDING);
        break;
	}
}

// Function that sorts all the materials in the tab by name
// 3 possible sorts: No sort; Ascending; Descending
void MaterialTracker::SortByName()
{
    // Throw an error if there are no active tabs
    if(materialTabs.size() <= 0)
    {
        error.NonModalErrorMessage(this, "Error", "No Active Tabs");
        return;
    }

    int currentTab = ui->materialsTabWidget->currentIndex();

	// Retrieve the next sort state when the sorting button is pressed
	// Once retrieved run the sorting algorithm and set the current sort state
	// and the next sort state for when the button is pressed again
    switch(materialTabs[currentTab]->GetNextSortState())
    {
    case MaterialStatusBar::SortingState::NAME_NONE:
        // No filter
        materialTabs[currentTab]->SortNoFilter();
        ui->sortNamePushButton->setText("Sort Name");
        materialTabs[currentTab]->SetCurrentSortState(MaterialStatusBar::SortingState::NAME_NONE);
        materialTabs[currentTab]->SetNextSortState(MaterialStatusBar::SortingState::NAME_ASCENDING);
        break;

    case MaterialStatusBar::SortingState::NAME_ASCENDING:
        // Lowest-Highest
        materialTabs[currentTab]->SortLowToHighName();
        ui->sortNamePushButton->setText("Sort Name - Ascending");
        materialTabs[currentTab]->SetCurrentSortState(MaterialStatusBar::SortingState::NAME_ASCENDING);
        materialTabs[currentTab]->SetNextSortState(MaterialStatusBar::SortingState::NAME_DESCENDING);
        break;

    case MaterialStatusBar::SortingState::NAME_DESCENDING:
        // Highest-Lowest
        materialTabs[currentTab]->SortHighToLowName();
        ui->sortNamePushButton->setText("Sort Name - Descending");
        materialTabs[currentTab]->SetCurrentSortState(MaterialStatusBar::SortingState::NAME_DESCENDING);
        materialTabs[currentTab]->SetNextSortState(MaterialStatusBar::SortingState::NAME_NONE);
        break;

    default:
		// Lowest-Highest
        ResetSortingButtons();
		materialTabs[currentTab]->SortLowToHighName();
		ui->sortNamePushButton->setText("Sort Name - Ascending");
        materialTabs[currentTab]->SetCurrentSortState(MaterialStatusBar::SortingState::NAME_ASCENDING);
		materialTabs[currentTab]->SetNextSortState(MaterialStatusBar::SortingState::NAME_DESCENDING);
        break;
    }
}

// Function that sorts all the materials in the tab by current amount
// 3 possible sorts: No sort; Ascending; Descending
void MaterialTracker::SortByCurrent()
{
    // Throw an error if there are no active tabs
    if(materialTabs.size() <= 0)
    {
        error.NonModalErrorMessage(this, "Error", "No Active Tabs");
        return;
    }

    int currentTab = ui->materialsTabWidget->currentIndex();

	// Retrieve the next sort state when the sorting button is pressed
	// Once retrieved run the sorting algorithm and set the current sort state
	// and the next sort state for when the button is pressed again
    switch(materialTabs[currentTab]->GetNextSortState())
    {
    case MaterialStatusBar::SortingState::CURRENT_NONE:
        // No filter
        materialTabs[currentTab]->SortNoFilter();
        ui->sortCurrentPushButton->setText("Sort Current");
        materialTabs[currentTab]->SetCurrentSortState(MaterialStatusBar::SortingState::CURRENT_NONE);
        materialTabs[currentTab]->SetNextSortState(MaterialStatusBar::SortingState::CURRENT_ASCENDING);
        break;

    case MaterialStatusBar::SortingState::CURRENT_ASCENDING:
        // Lowest-Highest
        materialTabs[currentTab]->SortLowToHighCurrent();
        ui->sortCurrentPushButton->setText("Sort Current - Ascending");
        materialTabs[currentTab]->SetCurrentSortState(MaterialStatusBar::SortingState::CURRENT_ASCENDING);
        materialTabs[currentTab]->SetNextSortState(MaterialStatusBar::SortingState::CURRENT_DESCENDING);
        break;

    case MaterialStatusBar::SortingState::CURRENT_DESCENDING:
        // Highest-Lowest
        materialTabs[currentTab]->SortHighToLowCurrent();
        ui->sortCurrentPushButton->setText("Sort Current - Descending");
        materialTabs[currentTab]->SetCurrentSortState(MaterialStatusBar::SortingState::CURRENT_DESCENDING);
        materialTabs[currentTab]->SetNextSortState(MaterialStatusBar::SortingState::CURRENT_NONE);
        break;

    default:
		// Lowest-Highest
        ResetSortingButtons();
		materialTabs[currentTab]->SortLowToHighCurrent();
		ui->sortCurrentPushButton->setText("Sort Current - Ascending");
        materialTabs[currentTab]->SetCurrentSortState(MaterialStatusBar::SortingState::CURRENT_ASCENDING);
		materialTabs[currentTab]->SetNextSortState(MaterialStatusBar::SortingState::CURRENT_DESCENDING);
        break;
    }
}

// Function that sorts all the materials in the tab by goal amount
// 3 possible sorts: No sort; Ascending; Descending
void MaterialTracker::SortByGoal()
{
    // Throw an error if there are no active tabs
    if(materialTabs.size() <= 0)
    {
        error.NonModalErrorMessage(this, "Error", "No Active Tabs");
        return;
    }

    int currentTab = ui->materialsTabWidget->currentIndex();

	// Retrieve the next sort state when the sorting button is pressed
	// Once retrieved run the sorting algorithm and set the current sort state
	// and the next sort state for when the button is pressed again
    switch(materialTabs[currentTab]->GetNextSortState())
    {
    case MaterialStatusBar::SortingState::GOAL_NONE:
        // No filter
        materialTabs[currentTab]->SortNoFilter();
        ui->sortGoalPushButton->setText("Sort Goal");
        materialTabs[currentTab]->SetCurrentSortState(MaterialStatusBar::SortingState::GOAL_NONE);
        materialTabs[currentTab]->SetNextSortState(MaterialStatusBar::SortingState::GOAL_ASCENDING);
        break;

    case MaterialStatusBar::SortingState::GOAL_ASCENDING:
        // Lowest-Highest
        materialTabs[currentTab]->SortLowToHighGoal();
        ui->sortGoalPushButton->setText("Sort Goal - Ascending");
        materialTabs[currentTab]->SetCurrentSortState(MaterialStatusBar::SortingState::GOAL_ASCENDING);
        materialTabs[currentTab]->SetNextSortState(MaterialStatusBar::SortingState::GOAL_DESCENDING);
        break;

    case MaterialStatusBar::SortingState::GOAL_DESCENDING:
        // Highest-Lowest
        materialTabs[currentTab]->SortHighToLowGoal();
        ui->sortGoalPushButton->setText("Sort Goal - Descending");
        materialTabs[currentTab]->SetCurrentSortState(MaterialStatusBar::SortingState::GOAL_DESCENDING);
        materialTabs[currentTab]->SetNextSortState(MaterialStatusBar::SortingState::GOAL_NONE);
        break;

    default:
		// Lowest-Highest
        ResetSortingButtons();
		materialTabs[currentTab]->SortLowToHighGoal();
		ui->sortGoalPushButton->setText("Sort Goal - Ascending");
        materialTabs[currentTab]->SetCurrentSortState(MaterialStatusBar::SortingState::GOAL_ASCENDING);
		materialTabs[currentTab]->SetNextSortState(MaterialStatusBar::SortingState::GOAL_DESCENDING);
		break;
    }
}

// Function that sorts all the materials in the tab by percent complete
// 3 possible sorts: No sort; Ascending; Descending
void MaterialTracker::SortByPercent()
{
    // Throw an error if there are no active tabs
    if(materialTabs.size() <= 0)
    {
        error.NonModalErrorMessage(this, "Error", "No Active Tabs");
        return;
    }

    int currentTab = ui->materialsTabWidget->currentIndex();

	// Retrieve the next sort state when the sorting button is pressed
	// Once retrieved run the sorting algorithm and set the current sort state
	// and the next sort state for when the button is pressed again
    switch(materialTabs[currentTab]->GetNextSortState())
    {
    case MaterialStatusBar::SortingState::PERCENT_NONE:
        // No filter
        materialTabs[currentTab]->SortNoFilter();
        ui->sortPercentPushButton->setText("Sort Percent");
        materialTabs[currentTab]->SetCurrentSortState(MaterialStatusBar::SortingState::PERCENT_NONE);
        materialTabs[currentTab]->SetNextSortState(MaterialStatusBar::SortingState::PERCENT_ASCENDING);
        break;

    case MaterialStatusBar::SortingState::PERCENT_ASCENDING:
        // Lowest-Highest Percentage
        materialTabs[currentTab]->SortLowToHighPercent();
        ui->sortPercentPushButton->setText("Sort Percent - Ascending");
        materialTabs[currentTab]->SetCurrentSortState(MaterialStatusBar::SortingState::PERCENT_ASCENDING);
        materialTabs[currentTab]->SetNextSortState(MaterialStatusBar::SortingState::PERCENT_DESCENDING);
        break;

    case MaterialStatusBar::SortingState::PERCENT_DESCENDING:
        // Highest-Lowest Percentage
        materialTabs[currentTab]->SortHighToLowPercent();
        ui->sortPercentPushButton->setText("Sort Percent - Descending");
        materialTabs[currentTab]->SetCurrentSortState(MaterialStatusBar::SortingState::PERCENT_DESCENDING);
        materialTabs[currentTab]->SetNextSortState(MaterialStatusBar::SortingState::PERCENT_NONE);
        break;

    default:
		// Lowest-Highest Percentage
        ResetSortingButtons();
		materialTabs[currentTab]->SortLowToHighPercent();
		ui->sortPercentPushButton->setText("Sort Percent - Ascending");
        materialTabs[currentTab]->SetCurrentSortState(MaterialStatusBar::SortingState::PERCENT_ASCENDING);
		materialTabs[currentTab]->SetNextSortState(MaterialStatusBar::SortingState::PERCENT_DESCENDING);
		break;
    }
}

/*************************************************************************
 *                            PRIVATE FUNCTIONS                          *
 *************************************************************************/

void MaterialTracker::SetIcons()
{
    QDir iconDir;

	// Set icons
	ui->materialsTabWidget->setStyleSheet(QString("QTabBar::close-button { image: url(%1/icons/close_tab_512.png) }")
		                                  .arg(iconDir.currentPath()));
}

// Function that resets all the sorting push buttons to their
// default text
void MaterialTracker::ResetSortingButtons()
{
	ui->sortCategoryPushButton->setText("Sort Category");
	ui->sortNamePushButton->setText("Sort Name");
	ui->sortCurrentPushButton->setText("Sort Current");
	ui->sortGoalPushButton->setText("Sort Goal");
	ui->sortPercentPushButton->setText("Sort Percent");
}

void MaterialTracker::CreateTabDataTable(QString name)
{
    DataInterface* dataInterface = new DataInterface;
    dataInterface->CreateDataTableForNewTab(name);
    delete dataInterface;
    setTabName(name);
}

void MaterialTracker::DeleteTabDataTable(QString name)
{
    DataInterface* dataInterface = new DataInterface;
    dataInterface->DeleteTabDataTable(name);
    delete dataInterface;
}

// Iterate through all the current tab names and remove
// them from their tables in the database
void MaterialTracker::DeleteAllTabDataTables()
{
    DataInterface* dataInterface = new DataInterface;
    QVector<QString> names = GetAllTabNames();
    for(auto tabName : names)
    {
        dataInterface->DeleteTabDataTable(tabName);
    }
    delete dataInterface;
}

void MaterialTracker::CreateTabsFromFile(int tabAmt,
                                         const QVector<QString>& tabNames,
                                         const QVector<int>& amtOfMatsInTab,
                                         const QVector<QVector<QString>>& namesInTab,
                                         const QVector<QVector<int>>& currentAmtsInTab,
                                         const QVector<QVector<int>>& goalAmtsInTab)
{
    // Remove all active tabs before loading new tabs in
    DeleteAllTabDataTables();
    ResetSortingButtons();
    // Clear the widget tab and vector tab
    ui->materialsTabWidget->clear();
    materialTabs.clear();

    for(int i = 0; i < tabAmt; i++)
    {
        // Process other events while loading files
        // and update status bar to show user the files are loading
        ChangeLoadingFileStatusLabel();

        // Create the corresponding data
        CreateTabDataTable(tabNames[i]);

        MaterialStatusBar* material = new MaterialStatusBar;
        material->SetTabName(tabNames[i]);
        if(amtOfMatsInTab[i] > 0)
        {
            material->AddMaterialFromSaveFile(amtOfMatsInTab[i], namesInTab[i],
                                              currentAmtsInTab[i], goalAmtsInTab[i]);
        }
        materialTabs.append(material);
        ui->materialsTabWidget->addTab(material, tabNames[i]);
    }

}

// Updates the status label depending on the int passed to it
// after saving a file
void MaterialTracker::CheckSaveFileStatus(int result)
{
    switch(result)
    {
    case 0:
        ui->fileStatusLabel->setText("Cancelled Saving File");
        break;

    case 1:
        ui->fileStatusLabel->setText("File Saved");
        break;

    case -1:
        ui->fileStatusLabel->setText("Error Saving File");
        break;
    }
}

// Changes the status label while loading in file
void MaterialTracker::ChangeLoadingFileStatusLabel()
{
    // Keep track of the amount of dots to add to the status label
    static int numOfDots = 1;
    switch(numOfDots)
    {
    case 1:
        ui->fileStatusLabel->setText(QString("Loading File."));
        numOfDots++;
        break;

    case 2:
        ui->fileStatusLabel->setText(QString("Loading File.."));
        numOfDots++;
        break;

    case 3:
        ui->fileStatusLabel->setText(QString("Loading File..."));
        numOfDots = 1;
        break;
    }
}

// Changes the status label while updating materials
void MaterialTracker::ChangeMaterialUpdatingStatusLabel()
{
    // Keep track of the amount of dots to add to the status label
    static int numOfDots = 1;
    switch(numOfDots)
    {
    case 1:
        ui->fileStatusLabel->setText(QString("Updating Materials."));
        numOfDots++;
        break;

    case 2:
        ui->fileStatusLabel->setText(QString("Updating Materials.."));
        numOfDots++;
        break;

    case 3:
        ui->fileStatusLabel->setText(QString("Updating Materials..."));
        numOfDots = 1;
        break;
    }
}

void MaterialTracker::SetupRecipeWordCompleter()
{
    // Fetch all recipe names from the database
    DataInterface dataInterface;
    QVector<QString> recipeNames;
    recipeNames = dataInterface.FetchAllRecipeNames();
    for (QString recipe : recipeNames) { recipeList.append(recipe); }
    
    // Create the word completer
    wordCompleter = new QCompleter(recipeList, this);
    wordCompleter->setFilterMode(Qt::MatchContains);
    wordCompleter->setCaseSensitivity(Qt::CaseInsensitive);
}
