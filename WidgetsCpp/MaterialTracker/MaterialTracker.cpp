#include "WidgetsHeader/MaterialTracker/MaterialTracker.h"
#include "ui_MaterialTracker.h"

MaterialTracker::MaterialTracker(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MaterialTracker),
    materialTabs(), tabName()
{
    // Setup
    ui->setupUi(this);

    // Set icons
    ui->materialsTabWidget->setStyleSheet(QString("QTabBar::close-button { image: url(%1/icons/close_tab_512.png) }")
                                          .arg(iconDir.currentPath()));

    // Signals/Slots
    connect(ui->addTabPushButton, &QPushButton::clicked, this, &MaterialTracker::AddTab);
    connect(ui->materialsTabWidget->tabBar(), SIGNAL(tabCloseRequested(int)), this, SLOT(RemoveTab(int)));
    connect(ui->materialsTabWidget->tabBar(), SIGNAL(tabBarDoubleClicked(int)), this, SLOT(RenameTab(int)));
    connect(ui->updatePushButton, &QPushButton::clicked, this, &MaterialTracker::UpdateMaterials);
    // Save n Load
    connect(ui->savePushButton, &QPushButton::clicked, this, &MaterialTracker::SaveToFile);
    connect(ui->loadPushButton, &QPushButton::clicked, this, &MaterialTracker::LoadFromFile);

}

MaterialTracker::~MaterialTracker()
{
    delete ui;
    DeleteAllTabDataTables();
    foreach(auto x, materialTabs) {delete x;}
}

/*************************************************************************
 *                            PUBLIC SLOTS                               *
 *************************************************************************/

// SLOT Function:
// Writes out all the current tabs and each material being tracked
// to file
void MaterialTracker::SaveToFile()
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
    SaveAndLoad* saveFile = new SaveAndLoad;
    int successResult = saveFile->SaveTrackedMaterialsToFile(this, tabAmt, vTabNames, vAmtOfMatInTab,
                                         vNamesInTab, vCurrentAmtsInTab,
                                         vGoalAmtsInTab);
    // Updates the status label
    // -1 = unsuccesful, 0 = cancelled, 1 = successful
    CheckSaveFileStatus(successResult);
}

// SLOT Function:
// Reads in all the current tabs and each material being tracked
// to file
void MaterialTracker::LoadFromFile()
{
    ui->fileStatusLabel->setText("Loading File...");

    // Loads in the file and retrieves the returned
    // QByteArray data and then adds that data
    // to a QTextStream to be easily parsed through
    // line by line
    SaveAndLoad* loadFile = new SaveAndLoad;
    QByteArray inFile = loadFile->LoadTrackedMaterialsFromFile(this);
    QTextStream fileResult(&inFile);

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
    bool isValidName = CheckIfValidTabName(name);
    if(!isValidName)
    {
        QMessageBox::warning(this, "Invalid Name", "Invalid Name");
        return;
    }
    else if(isOk && !name.isEmpty())
    {
        // Create the data table for the new tab
        CreateTabDataTable(name);

        // Creates a new MaterialStatusBar widget,
        // adds it to the data structure, and
        // creates a new tab with the widget
        MaterialStatusBar* material = new MaterialStatusBar;
        material->SetTabName(name); // Set the tab name for that stores this widget
        materialTabs.append(material);
        ui->materialsTabWidget->addTab(material, name);
    }
}

// SLOT Function:
// Triggers whenever the user hits the 'X' icon on the tab
void MaterialTracker::RemoveTab(int index)
{
    DeleteTabDataTable(ui->materialsTabWidget->tabText(index));
    ui->materialsTabWidget->removeTab(index);
    materialTabs.remove(index);
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
    bool isValidName = CheckIfValidTabName(name);
    if(!isValidName)
    {
        QMessageBox::warning(this, "Invalid Name", "Invalid Name");
        return;
    }
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
    // Query the network
    ApiAccess* api = new ApiAccess;
    api->QueryForMaterialsAPI(this);

    // After retrieving the API data
    // up all the values for the ui
    for(auto tab : materialTabs)
    {
        tab->UpdateAllMaterials();
        // Show that we are waiting for the
        // API reply
        ChangeMaterialUpdatingStatusLabel();
    }

    ui->fileStatusLabel->setText(QString("Materials Updated Successfully"));
}

/*************************************************************************
 *                           PUBLIC FUNCTIONS                            *
 *************************************************************************/

void MaterialTracker::ChangeWaitingForApiReplyStatusLabel()
{
    // Keep track of the amount of dots to add to the status label
    static int numOfDots = 1;
    switch(numOfDots)
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
    for(auto material : materialTabs)
    {
        tabNames.append(material->GetTabName());
    }

    return tabNames;
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

// Checks if the tab name that the user entered is valid
// use. There are certain checks that must be made in order
// for the user entered tab name to be reflected in the
// SQLite database.
bool MaterialTracker::CheckIfValidTabName(QString name)
{
    if(name.contains("-") || name.contains("."))
    {
        return false;
    }

    // Check if name starts with a number
    QString numbers = "0123456789";
    for(int i = 0; i < numbers.size(); i++)
    {
        if(name[0] == numbers[i])
        {
            return false;
        }
    }

    // Check for existing tab names
    for(int i = 0; i < materialTabs.size(); i++)
    {
        // Strip the new tab name and the current tab name
        // down to what SQLite sees to make sure that
        // there are no conflicting names
        QString tempNewName = name.simplified();
        tempNewName.replace(" ", "");

        QString tempCurrentName = materialTabs[i]->GetTabName().simplified();
        tempCurrentName.replace(" ", "");

        if(tempNewName.toLower() == tempCurrentName.toLower())
        {
            return false;
        }
    }

    // Name is valid
    return true;
}

/*************************************************************************
 *                            PRIVATE FUNCTIONS                          *
 *************************************************************************/

void MaterialTracker::CreateTabsFromFile(int tabAmt,
                                         QVector<QString> tabNames,
                                         QVector<int> amtOfMatsInTab,
                                         QVector<QVector<QString>> namesInTab,
                                         QVector<QVector<int>> currentAmtsInTab,
                                         QVector<QVector<int>> goalAmtsInTab)
{
    // Remove all active tabs before loading new tabs in
    DeleteAllTabDataTables();
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
            material->AddMaterialFromFile(amtOfMatsInTab[i], namesInTab[i],
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
        ui->fileStatusLabel->setText("Saving Cancelled");
        break;

    case 1:
        ui->fileStatusLabel->setText("Saving Successful");
        break;

    case -1:
        ui->fileStatusLabel->setText("Saving Failed");
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

