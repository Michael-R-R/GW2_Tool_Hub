#include "WidgetsHeader/Notes/Notes.h"
#include "ui_Notes.h"

Notes::Notes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Notes), listModel(new QFileSystemModel), menu(new QMenu(this)),
    dir(dir.currentPath()), folderDir(dir.path() + "/notes"),
    lastFolderDir(), htmlDir(), folderHistory(), folderName("")
{
    // Setup
    ui->setupUi(this);
    SetupListViewMenu();
    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->listView->setModel(listModel);
    ui->listView->setRootIndex(listModel->setRootPath(folderDir));
    ui->dirStatusLabel->setText(folderDir);

    // SIGNALS/SLOTS
    connect(ui->backPushButton, &QPushButton::clicked, this, &Notes::BackDirectory);
    connect(ui->listView, SIGNAL(clicked(QModelIndex)), this, SLOT(SingleClickItem(QModelIndex)));
    connect(ui->listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(DoubleClickItem(QModelIndex)));
    // Change text color
    connect(ui->whitePushButton, &QPushButton::clicked, this, &Notes::SetTextWhite);
    connect(ui->redPushButton, &QPushButton::clicked, this, &Notes::SetTextRed);
    connect(ui->greenPushButton, &QPushButton::clicked, this, &Notes::SetTextGreen);
    connect(ui->bluePushButton, &QPushButton::clicked, this, &Notes::SetTextBlue);
    // Custom Menu
    connect(ui->listView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ListViewCustomMenu(QPoint)));
}


Notes::~Notes()
{
    delete ui;
    delete listModel;
    delete menu;
}

// Saves the current text inside the textEdit
void Notes::SaveNotes()
{
    SaveAndLoad* saveFile = new SaveAndLoad;
    saveFile->SaveNotes(this, ui->textEdit->toHtml());
    delete saveFile;
}

// Loads in html text
void Notes::OpenNotes()
{
    SaveAndLoad* openFile = new SaveAndLoad;
    QString fileContent = openFile->LoadNotes(this);

    ui->textEdit->clear();
    ui->textEdit->setHtml(fileContent);

    delete openFile;
}

// If user single clicks on an item in the listView
// and is a folder, saves the folder name. Used in order
// if the user wants to delete the selected folder
void Notes::SingleClickItem(QModelIndex item)
{
    QString itemName = item.data().toString();
    // Is folder
    if(!itemName.contains("."))
    {
        // Allow user to select the delete folder menu option
        menu->actions().at(1)->setDisabled(false);
        folderName = itemName;
    }
    // Is not folder
    else
    {
        // Disable the delete folder menu option
        menu->actions().at(1)->setDisabled(true);
        folderName = "";
    }
}

// Looks for double click on listView items
// If folder: go into that directory
// If .html file: open the file in the textEdit
void Notes::DoubleClickItem(QModelIndex item)
{
    QString tempDir = listModel->fileInfo(item).absoluteFilePath();

    // Doesn't contain a dot
    // is a folder, go into folder
    if(!tempDir.contains("."))
    {
        // Save the last folder directory
        // to allow user to go back to that directory
        lastFolderDir = folderDir;
        folderHistory.append(lastFolderDir);

        // Go to the selected directory
        folderDir = tempDir;
        ui->listView->setRootIndex(listModel->setRootPath(folderDir));

        // Update directory status label to current directory
        // and disable delete folder menu option
        ui->dirStatusLabel->setText(folderDir);
        menu->actions().at(1)->setDisabled(true);
    }
    // Is .html file, open it
    else if(tempDir.contains(".html"))
    {
        htmlDir = tempDir;

        QFile inFile(htmlDir);
        if(!inFile.open(QIODevice::ReadOnly))
        {
            error.NonModalErrorMessage(this, "Unable to open file", inFile.errorString());
            return;
        }

        QString fileContent = inFile.readAll();
        ui->textEdit->setHtml(fileContent);

        // Set cursor back to the start
        QTextCursor cursor = ui->textEdit->textCursor();
        cursor.setPosition(0);
        ui->textEdit->setTextCursor(cursor);

        // Close file
        inFile.close();
    }
    else
    {
        qDebug() << "Invalid file";
        return;
    }

}

// Go back to the previous directory visited
void Notes::BackDirectory()
{
    if(folderHistory.size() > 0)
    {
        int index = folderHistory.size() - 1;

        ui->listView->setRootIndex(listModel->setRootPath(folderHistory[index]));
        folderDir = listModel->rootPath();
        ui->dirStatusLabel->setText(folderDir);
        folderHistory.removeLast();
    }
}

void Notes::SetTextWhite()
{
    ui->textEdit->setTextColor(QColor(255, 255, 255));
    ui->textEdit->setFocus();
}

void Notes::SetTextRed()
{
    ui->textEdit->setTextColor(QColor(255, 0, 0));
    ui->textEdit->setFocus();
}

void Notes::SetTextGreen()
{
    ui->textEdit->setTextColor(QColor(0, 255, 0));
    ui->textEdit->setFocus();
}

void Notes::SetTextBlue()
{
    ui->textEdit->setTextColor(QColor(0, 0, 255));
    ui->textEdit->setFocus();
}

// Shows the popup menu when user right clicks inside the
// listView
void Notes::ListViewCustomMenu(const QPoint &pos)
{
    menu->popup(ui->listView->viewport()->mapToGlobal(pos));
}

// Menu option:
// Allow user to create a new folder (directory)
void Notes::CreateFolder()
{
    bool isOk = false;
    QString name = QInputDialog::getText(this, tr("Folder Name"),
                                               tr("Folder Name"), QLineEdit::Normal,
                                               tr(""), &isOk);

    if(isOk && !name.isEmpty())
    {
        QDir newDir;
        newDir.mkdir(listModel->rootPath() + "/" + name);
    }
}

// Menu Option:
// Deletes selected folder
void Notes::DeleteFolder()
{
    // Yes or no popup message box
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete Folder", "Delete " + folderName + " Folder?",
                          QMessageBox::Yes | QMessageBox::No);
    if(reply == QMessageBox::Yes)
    {
        QDir deleteDir(listModel->rootPath() + "/" + folderName);
        deleteDir.removeRecursively();
    }
    else
    {
        qDebug() << "don't delete folder";
        return;
    }
}

// Creates all the options for the listView popup
// menu
void Notes::SetupListViewMenu()
{
    menu->addAction(tr("Create Folder"), this, SLOT(CreateFolder()));
    menu->addAction(tr("Delete Folder"), this, SLOT(DeleteFolder()));

    // Set 'Delete Folder' to disabled
    menu->actions().at(1)->setDisabled(true);
}

// Checks to see if the user pressed the RMB
// If RMB pressed inside listView, open the popup menu
void Notes::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        connect(ui->listView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(ListViewCustomMenu(const QPoint &)));
    }
}
