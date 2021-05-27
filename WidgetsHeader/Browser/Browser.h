#ifndef BROWSER_H
#define BROWSER_H

#include <QWidget>
#include <QString>
#include <QVector>
#include <QUrl>
#include <QMessageBox>
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
    void LoadYoutubePage();
    void LoadPreviousPages();
    void LoadForwardPage();
    void RefreshPage();

signals:


private:
    Ui::Browser *ui;

    // --- Variables ---
    QUrl lastPageVisited;

    // --- Functions ---
    void NonModalErrorMessage(QString title, QString text, bool isModal);
    bool CheckForValidPage(const QUrl& url);

};

#endif // BROWSER_H
