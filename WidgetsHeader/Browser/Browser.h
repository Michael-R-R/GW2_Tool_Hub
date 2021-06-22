#ifndef BROWSER_H
#define BROWSER_H

#include "NonWidgetsHeader/ErrorHandling.h"
#include <QWidget>
#include <QString>
#include <QVector>
#include <QUrl>
#include <QWebEngineProfile>
#include <QWebEngineCookieStore>
#include <QWebEngineHistory>


namespace Ui { class Browser; }

class Browser : public QWidget
{
    Q_OBJECT

public:
    Browser(QWidget *parent = nullptr);
    ~Browser();

    // --- Getters ---
    QUrl GetLastPageVisited() const { return lastPageVisited; }

    // --- Utility Functions ---
    void DeleteCookies();
    void DeleteVisitedLinks();
    void DeleteHistory();

private slots:
    void SetLoadingPercent(int percent);
    void SetLastPageVisited(const QUrl& url);
    void LoadWikiHomePage();
    void LoadSnowCrowsPage();
    void LoadGW2EfficiencyPage();
    void LoadGW2CraftsPage();
    void LoadYoutubePage();
    void LoadPreviousPages();
    void LoadForwardPage();
    void RefreshPage();

signals:


private:
    Ui::Browser *ui;

    // --- Error Handling ---
    ErrorHandling error;

    // --- Variables ---
    QUrl lastPageVisited;

};

#endif // BROWSER_H
