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
    connect(ui->gw2CraftsPushButton, &QPushButton::clicked, this, &Browser::LoadGW2CraftsPage);
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
    if(!error.CheckForValidPage(this, url))
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

void Browser::LoadGW2CraftsPage()
{
    ui->webEngineView->setUrl(QUrl("https://gw2crafts.net/"));
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









