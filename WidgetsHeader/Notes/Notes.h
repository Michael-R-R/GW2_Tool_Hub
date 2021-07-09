#ifndef NOTES_H
#define NOTES_H

#include "NonWidgetsHeader/SaveAndLoad.h"
#include "NonWidgetsHeader/ErrorHandling.h"
#include <QWidget>
#include <QFileSystemModel>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QTextCursor>
#include <QMouseEvent>
#include <QMenu>
#include <QDebug>

namespace Ui {
class Notes;
}

class Notes : public QWidget
{
    Q_OBJECT

public:
    explicit Notes(QWidget *parent = nullptr);
    ~Notes();

public slots:
    void SaveNotes();
    void SaveAsNotes();
    void OpenNotes();

private slots:
    void SingleClickItem(QModelIndex item);
    void DoubleClickItem(QModelIndex item);
    void BackDirectory();

    void SetTextWhite();
    void SetTextRed();
    void SetTextGreen();
    void SetTextBlue();

    void ListViewCustomMenu(const QPoint &pos);
    void CreateFolder();
    void DeleteFolder();

signals:

private:
    Ui::Notes *ui;
    QFileSystemModel *listModel;
    QMenu* menu;
    SaveAndLoad* saveAndLoad;

    // --- Error Handling ---
    ErrorHandling error;

    // --- Directory ---
    QDir dir;
    QString folderDir;
    QString lastFolderDir;
    QString htmlDir;
    QVector<QString> folderHistory;

    QString folderName;

    // --- Menus ---
    void SetupListViewMenu();

    // --- Events --
    void mousePressEvent(QMouseEvent* event);

};

#endif // NOTES_H
