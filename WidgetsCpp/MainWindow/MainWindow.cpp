#include "WidgetsHeader/MainWindow/MainWindow.h"
#include "ui_MainWindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      dataInterface(new DataInterface),
      isNotesWindowOpen(false), notesWidget(new Notes),
      isMaterialTrackerWindowOpen(false), materialTrackerWidget(new MaterialTracker),
      isWikiBrowserOpen(false), lastPageVisited(), BrowserWidget(new Browser)
{
    // Setup
    ui->setupUi(this);
    SetWindowConfigs();

    // Allow moving window around by clicking&dragging menu bar
    ui->menuBar->installEventFilter(this);

    // Setup stacked widget
    SetupStackedWidget();

    // Signals/Slots: Menu bar
    // File:
    connect(ui->actionClose_Application, &QAction::triggered, this, &MainWindow::CloseApplication);
    // API:
    connect(ui->actionAdd_API_Key, &QAction::triggered, this, &MainWindow::AddApiKey);
    connect(ui->actionRemove_API_Key, &QAction::triggered, this, &MainWindow::RemoveApiKey);
    // Notes:
    connect(ui->notesActionOpen, &QAction::triggered, this, &MainWindow::OpenNotesFile);
    connect(ui->notesActionSave, &QAction::triggered, this, &MainWindow::SaveNotes);
    connect(ui->notesActionSaveAs, &QAction::triggered, this, &MainWindow::SaveAsNotes);
    // Material Tracker:
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::OpenMaterialTrackerFile);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::SaveMaterialTracker);
    connect(ui->matTraActionSaveAs, &QAction::triggered, this, &MainWindow::SaveAsMaterialTracker);
    connect(ui->actionSearchRecipe, &QAction::triggered, this, &MainWindow::SearchRecipe);
    connect(ui->actionImportExcelSheet, &QAction::triggered, this, &MainWindow::ImportExcelSheet);
    // Browser:
    connect(ui->actionDelete_Cookies, &QAction::triggered, this, &MainWindow::DeleteCookies);
    connect(ui->actionClear_All_Visited_Links, &QAction::triggered, this, &MainWindow::DeleteVisitedLinks);
    connect(ui->actionClear_History, &QAction::triggered, this, &MainWindow::DeleteHistory);

    // Signals/Slots: Open other GUI windows
    connect(ui->notesPushButton, &QPushButton::clicked, this, &MainWindow::ShowNotesWidget);
    connect(ui->materialTrackPushButton, &QPushButton::clicked, this, &MainWindow::ShowMaterialTrackerWidget);
    connect(ui->browserPushButton, &QPushButton::clicked, this, &MainWindow::ShowBrowserWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete dataInterface;
    delete notesWidget;
    delete materialTrackerWidget;
    delete BrowserWidget;
}

/*************************************************************************
 *                            PUBLIC SLOTS                               *
 *************************************************************************/

// File:
// Closes the application
// HotKey = Ctrl + e
void MainWindow::CloseApplication()
{
    QApplication::quit();
}

// API:
// Adds user API key when clicked from file menu
void MainWindow::AddApiKey()
{
    // Popup message to enter the API key
    bool ok = false;
    QString apiKey = QInputDialog::getText(this, tr("Add API Key"),
                                           tr("API Key"), QLineEdit::Password,
                                           tr(""), &ok);
    // Only enter the value if something
    // was entered and press 'okay'
    if(ok && !apiKey.isEmpty())
    {
        dataInterface->UpdateApiKey(apiKey);
    }
}

// API:
// Removes user API key from the database
// when clicked from file menu
void MainWindow::RemoveApiKey()
{
    dataInterface->UpdateApiKey("");

    QMessageBox::about(this, "API Key", "API Key Removed");
}

// Notes:
void MainWindow::SaveNotes()
{
    notesWidget->SaveNotes();
}

void MainWindow::SaveAsNotes()
{
    notesWidget->SaveAsNotes();
}

// Notes:
void MainWindow::OpenNotesFile()
{
    notesWidget->OpenNotes();
}

// Material Tracker:
// Saves all the tracked materials and their tabs to a .txt file
void MainWindow::SaveMaterialTracker()
{
     materialTrackerWidget->SaveFile();
}

void MainWindow::SaveAsMaterialTracker()
{
    materialTrackerWidget->SaveAsFile();
}

// Material Tracker:
// Loads in all tracked materials and their tabs
void MainWindow::OpenMaterialTrackerFile()
{
    materialTrackerWidget->OpenFile();
}

void MainWindow::SearchRecipe()
{
    materialTrackerWidget->SearchRecipe();
}

void MainWindow::ImportExcelSheet()
{
    materialTrackerWidget->ImportExcelSheet();
}

// Shows the Notes widget when the ui->notesPushButton is pressed
void MainWindow::ShowNotesWidget()
{
    // Only add one widget to the stackedWidget page
    if(!isNotesWindowOpen)
    {
        // Get the correct page index for the widget
        int index = ui->stackedWidget->indexOf(notesWidget);
        ui->stackedWidget->setCurrentIndex(index);
        ui->stackedWidget->show();
        // Only show the one widget
        SetAllWindowBoolChecks(true, false, false);
        // Set the fixed size of the application window
        setFixedHeight(winHeightOpen);
    }
    else
    {
        // Collapse the widget
        ui->stackedWidget->hide();
        setFixedHeight(winHeightClose);
        isNotesWindowOpen = false;
    }
}

// Show the Material Tracker widget when the ui->materialTrackerButton
// is pressed
void MainWindow::ShowMaterialTrackerWidget()
{
    // Only add one widget to the stackedWidget page
    if(!isMaterialTrackerWindowOpen)
    {
        // Get the correct page index for the widget
        int index = ui->stackedWidget->indexOf(materialTrackerWidget);
        ui->stackedWidget->setCurrentIndex(index);
        ui->stackedWidget->show();
        // Only show the one widget
        SetAllWindowBoolChecks(false, true, false);
        // Set the fixed size of the application window
        setFixedHeight(winHeightOpen);
    }
    else
    {
        // Collapse the widget
        ui->stackedWidget->hide();
        setFixedHeight(winHeightClose);
        isMaterialTrackerWindowOpen = false;
    }
}

// Show the Browser when the ui->wikiPushButton is pressed
void MainWindow::ShowBrowserWidget()
{
    // Only add one widget to the stackedWidget page
    if(!isWikiBrowserOpen)
    {
        // Get the correct page index for the widget and show it
        int index = ui->stackedWidget->indexOf(BrowserWidget);
        ui->stackedWidget->setCurrentIndex(index);
        ui->stackedWidget->show();
        // Only show the one widget
        SetAllWindowBoolChecks(false, false, true);
        // Set the fixed size of the application window
        setFixedHeight(winHeightOpen);
    }
    else
    {
        // Collapse the widget
        ui->stackedWidget->hide();
        setFixedHeight(winHeightClose);
        isWikiBrowserOpen = false;
    }
}

// Menu bar item: deletes all cookies
void MainWindow::DeleteCookies()
{
    BrowserWidget->DeleteCookies();
}

// Menu bar item: deletes all visited links
void MainWindow::DeleteVisitedLinks()
{
    BrowserWidget->DeleteVisitedLinks();
}

// Menu bar item: Deletes browser history
void MainWindow::DeleteHistory()
{
    BrowserWidget->DeleteHistory();
}

/*************************************************************************
 *                      PRIVATE FUNCTIONS                                *
 *************************************************************************/

// Allows clicking and dragging the application by the menu bar
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->menuBar)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->button() == Qt::LeftButton)
            {
                dragPosition = mouse_event->globalPos() - frameGeometry().topLeft();
                return false;
            }
        }
        else if (event->type() == QEvent::MouseMove)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton)
            {
                move(mouse_event->globalPos() - dragPosition);
                return false;
            }
        }
    }
    return false;
}

// Sets the active/inactive status of each widget window
void MainWindow::SetAllWindowBoolChecks(bool notes, bool materialTracker, bool wikiBrowser)
{
    isNotesWindowOpen = notes;
    isMaterialTrackerWindowOpen = materialTracker;
    isWikiBrowserOpen = wikiBrowser;

    // Enable/Disable menu bar options
    SetMaterialTrackerMenuItemsStatus(materialTracker);
    SetNotesMenuItemStatus(notes);
}

// Enables or disables the menu bar items for Material Tracker
// disabled if the window is not active and enabled if so
void MainWindow::SetMaterialTrackerMenuItemsStatus(bool state)
{
    if(state == true)
    {
        ui->actionOpen->setEnabled(true);
        ui->actionSave->setEnabled(true);
        ui->matTraActionSaveAs->setEnabled(true);
        ui->actionSearchRecipe->setEnabled(true);
        ui->actionImportExcelSheet->setEnabled(true);
    }
    else
    {
        ui->actionOpen->setDisabled(true);
        ui->actionSave->setDisabled(true);
        ui->matTraActionSaveAs->setDisabled(true);
        ui->actionSearchRecipe->setDisabled(true);
        ui->actionImportExcelSheet->setDisabled(true);
    }
}

// Enables or disables the menu bar items for Notes Tool
// disabled if the window is not active and enabled if so
void MainWindow::SetNotesMenuItemStatus(bool state)
{
    if(state == true)
    {
        ui->notesActionOpen->setEnabled(true);
        ui->notesActionSave->setEnabled(true);
        ui->notesActionSaveAs->setEnabled(true);
    }
    else
    {   
        ui->notesActionOpen->setDisabled(true);
        ui->notesActionSave->setDisabled(true);
        ui->notesActionSaveAs->setDisabled(true);
    }
}

// Set the options for the window: I.E. Always on top, frame less, etc.
// Set the windows icons
void MainWindow::SetWindowConfigs()
{
    // Set window options config
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    // Set icons
    setWindowIcon(QIcon(iconDir.currentPath() + "/icons/window_icon.ico"));
}

// Adds all the widgets to the stacked widget
void MainWindow::SetupStackedWidget()
{
    ui->stackedWidget->hide();
    ui->stackedWidget->addWidget(notesWidget);
    ui->stackedWidget->addWidget(materialTrackerWidget);
    ui->stackedWidget->addWidget(BrowserWidget);
}

/*************************************************************************
 *                        DEVELOPER FUNCTIONS                            *
 *************************************************************************/

// Queries for each material in the materialCatalog, retrieves
// the icon url, saves the .png, and stores it in the database
void MainWindow::FetchAllMaterialIcons()
{
	// Fetches all the material id #s
	QVector<QString> ids = dataInterface->FetchAllMaterialIds();

	// Fetches the material icon and stores in pixmap
	ApiAccess* api = new ApiAccess;
	// Add material icon to the database
	for (int i = 0; i < ids.size(); i++)
	{
		QPixmap icon = api->QueryForMaterialIcon(ids[i]);
		dataInterface->UpdateMaterialIcons(ids[i].toInt(), icon);
		qDebug() << "Row ID: " << i;
	}

	api->deleteLater();
}

















