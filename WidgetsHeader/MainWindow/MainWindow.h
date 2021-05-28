#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "WidgetsHeader/MaterialTracker/MaterialTracker.h"
#include "WidgetsHeader/Browser/Browser.h"
#include "WidgetsHeader/Notes/Notes.h"
#include "NonWidgetsHeader/ApiAccess.h"
#include "NonWidgetsHeader/DataInterface.h"
#include <QVector>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>
#include <QIcon>
#include <QDir>
#include <QtGui>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    // --- Menu Bar ---
    // File:
    void CloseApplication();
    // API:
    void AddApiKey();
    void RemoveApiKey();
    // Notes:
    void SaveNotes();
    void LoadNotes();
    // Material Tracker:
    void SaveMaterialTracker();
    void LoadMaterialTracker();
    void ImportExcelSheet();

    // --- Notes ---
    void ShowNotesWidget();

    // --- Material Tracker ---
    void ShowMaterialTrackerWidget();

    // --- Browser ---
    void ShowBrowserWidget();
    void DeleteCookies();
    void DeleteVisitedLinks();
    void DeleteHistory();

signals:


private:
    Ui::MainWindow *ui;
    QDir iconDir;

    // --- Variables ---
    int winHeightOpen = 500;
    int winHeightClose = 50;

    // --- Events ---
    // Move window by clicking and dragging Menubar
    QPoint dragPosition;
    bool eventFilter(QObject *watched, QEvent *event);

    // --- Utility Functions ---
    void SetAllWindowBoolChecks(bool notes, bool materialTracker, bool wikiBrowser);
    void SetMaterialTrackerMenuItemsStatus(bool state);
    void SetNotesMenuItemStatus(bool state);

    // --- Data interface ---
    DataInterface* dataInterface;

    // --- Notes ---
    bool isNotesWindowOpen;
    Notes* notesWidget;

    // --- Material Tracker ---
    bool isMaterialTrackerWindowOpen;
    MaterialTracker* materialTrackerWidget;

    // --- Browser ---
    bool isWikiBrowserOpen;
    QUrl lastPageVisited;
    Browser* BrowserWidget;

private:

    // --- Functions ---
    void SetWindowConfigs();
    void SetupStackedWidget();

};
#endif // MAINWINDOW_H
