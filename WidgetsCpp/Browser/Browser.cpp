#include "WidgetsHeader/Browser/Browser.h"
#include "ui_Browser.h"


Browser::Browser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Browser),
    lastPageVisited()
{
    ui->setupUi(this);

    // Zoom factor
    ui->webEngineView->setZoomFactor(0.75);

    // SIGNAL/SLOTS
    connect(ui->webEngineView, &QWebEngineView::loadProgress, this, &Browser::SetLoadingPercent);
    connect(ui->webEngineView, &QWebEngineView::urlChanged, this, &Browser::SetLastPageVisited);
    // Load requested pages
    connect(ui->wikiPushButton, &QPushButton::clicked, this, &Browser::LoadWikiHomePage);
    connect(ui->snowcrowsPushButton, &QPushButton::clicked, this, &Browser::LoadSnowCrowsPage);
    connect(ui->gw2efficiencyPushButton, &QPushButton::clicked, this, &Browser::LoadGW2EfficiencyPage);
    connect(ui->youtubePushButton, &QPushButton::clicked, this, &Browser::LoadYoutubePage);
    // Back/Forward/Reload Buttons
    connect(ui->backPushButton, &QPushButton::clicked, this, &Browser::LoadPreviousPages);
    connect(ui->forwardPushButton, &QPushButton::clicked, this, &Browser::LoadForwardPage);
    connect(ui->refreshPushButton, &QPushButton::clicked, this, &Browser::RefreshPage);
}

Browser::~Browser()
{
    delete ui;
}

/*************************************************************************
 *                        UTILITY FUNCTIONS                              *
 *************************************************************************/

void Browser::DeleteCookies()
{
    ui->webEngineView->page()->profile()->defaultProfile()->cookieStore()->deleteAllCookies();
}

void Browser::DeleteVisitedLinks()
{
    ui->webEngineView->page()->profile()->defaultProfile()->clearAllVisitedLinks();
}

void Browser::DeleteHistory()
{
    ui->webEngineView->history()->clear();
}

/*************************************************************************
 *                        PRIVATE SLOTS                                  *
 *************************************************************************/

// Sets the green progress bar whenever a url page is laoded
void Browser::SetLoadingPercent(int percent)
{
    ui->pageLoadingProgressBar->setValue(percent);
}

// Saves the url of the last page the user viewed
void Browser::SetLastPageVisited(const QUrl &url)
{
    // if not a valid page, load the last valid page
    if(!CheckForValidPage(url))
    {
        ui->webEngineView->page()->triggerAction(QWebEnginePage::Stop);
        ui->webEngineView->setUrl(lastPageVisited);
        return;
    }

    lastPageVisited = url;
}

void Browser::LoadWikiHomePage()
{
    ui->webEngineView->setUrl(QUrl("https://wiki.guildwars2.com/wiki/Main_Page"));
}

void Browser::LoadSnowCrowsPage()
{
    ui->webEngineView->setUrl(QUrl("https://snowcrows.com/"));
}

void Browser::LoadGW2EfficiencyPage()
{
    ui->webEngineView->setUrl(QUrl("https://gw2efficiency.com/"));
}

void Browser::LoadYoutubePage()
{
    ui->webEngineView->setUrl(QUrl("https://www.youtube.com/"));
}

void Browser::LoadPreviousPages()
{
    ui->webEngineView->page()->triggerAction(QWebEnginePage::Back);
}

void Browser::LoadForwardPage()
{
    ui->webEngineView->page()->triggerAction(QWebEnginePage::Forward);
}

void Browser::RefreshPage()
{
    ui->webEngineView->page()->triggerAction(QWebEnginePage::Reload);
}

/*************************************************************************
 *                             PRIVATE                                   *
 *************************************************************************/

// Creates a non modal pop up message in case of an error
// non-modal = does not block flow of execution
void Browser::NonModalErrorMessage(QString title, QString text, bool isModal)
{
    QMessageBox* message = new QMessageBox(this);
    message->setAttribute(Qt::WA_DeleteOnClose);
    message->setStandardButtons(QMessageBox::Ok);
    message->setWindowTitle(title);
    message->setText(text);
    message->setIcon(QMessageBox::Warning);
    message->setModal(isModal);
    message->open();
}

// Function to check if the user is going to a valid website address
// Returns true if valid, false if not
bool Browser::CheckForValidPage(const QUrl &url)
{
    // Check the url
    QString urlString = url.toString();
    if(urlString.contains("https://wiki.guildwars2.com")           ||
       urlString.contains("https://www.youtube.com")               ||
       urlString.contains("https://snowcrows.com")                 ||
       urlString.contains("https://gw2efficiency.com"))
    {
        return true;
    }

    // Non modal pop-up message (does not block flow of execution)
    NonModalErrorMessage("Error", "Invalid Page", false);
    return false;
}










