#include "NonWidgetsHeader/SaveAndLoad.h"

SaveAndLoad::SaveAndLoad() :
    fileName()
{

}

SaveAndLoad::~SaveAndLoad()
{

}

/*************************************************************************
 *                          PUBLIC FUNCTIONS                             *
 *************************************************************************/

// Reads in .txt file that was selected
// Returns: a QByteArray which keeps the file contents in tack
// to be read into a QTextStream for further processing
QByteArray SaveAndLoad::LoadTrackedMaterialsFromFile(QWidget* parent)
{

    fileName = QFileDialog::getOpenFileName(parent,
                                            tr("Open Tracked Materials"), "",
                                            tr("Tracked Materials (*.txt)"));
    // Open file if there is a valid file selected
    if(fileName.isEmpty()) { return ""; }
    else
    {
        // Check if we can open the file
        QFile in(fileName);
        if(!in.open(QIODevice::ReadOnly))
        {
            QMessageBox::information(parent, tr("Unable to open file"), in.errorString());
            return "";
        }

        // Return all the information from the file read in
        return in.readAll();
    }
}

// Writes out all the tracked materials in each tab to a file.
// Writes out: number of tabs, all tab names, number of materials in tab,
// all material names in tab, the material current and goal amount
// Returns: 1 = saved successfully, -1 = saved unsuccessfully, 0 = cancelled
int SaveAndLoad::SaveTrackedMaterialsToFile(QWidget* parent, int tabAmt,
                                             QVector<QString> tabNames,
                                             QVector<int> amtOfMatsInTab,
                                             QVector<QVector<QString>> namesInTab,
                                             QVector<QVector<int>> currentAmtsInTab,
                                             QVector<QVector<int>> goalAmtsInTab)
{
    // Open the file explorer to allow user to pick place to save
    // file
    fileName = QFileDialog::getSaveFileName(parent,
                                            tr("Save Tracked Materials"), "",
                                            tr("Tracked Materials (*.txt)"));
    // Check if file is empty, if so do nothing
    // otherwise write out the contents to file
    if(fileName.isEmpty()) { return 0; }
    else
    {
        // Check if the file was opened successfully
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::information(parent, tr("Unable to open file"),
                                             file.errorString());
            return -1;
        }

        // Start writing out the data
        QTextStream outFile(&file);

        outFile << "Amount of Tabs: " << tabAmt << "\n";
        for(int i = 0; i < tabAmt; i++)
        {
            outFile << "Tab Name: " << tabNames[i] << "\n";
            outFile << "Amount of Materials: " << amtOfMatsInTab[i] << "\n";
            // iterate through the 2D vectors
            for(int j = 0; j < amtOfMatsInTab[i]; j++)
            {
                outFile << "Material Name: " << namesInTab[i][j] << "\n";
                outFile << "Material Current Amt: " << currentAmtsInTab[i][j] << "\n";
                outFile << "Material Goal Amt: " << goalAmtsInTab[i][j] << "\n";
            }
        }
        file.close();
        return 1;
    }
}


QString SaveAndLoad::LoadNotes(QWidget* parent)
{
    // Opens the file explorer to select the file to load
    fileName = QFileDialog::getOpenFileName(parent,
                                            tr("Open Notes"), "",
                                            tr("Notes (*.html)"));
    // If no file was selected, return nothing
    if(fileName.isEmpty()) { return ""; }
    else
    {
        // Check if the file was opened successfully
        QFile in(fileName);
        if(!in.open(QIODevice::ReadOnly))
        {
            QMessageBox::information(parent, tr("Unable to open file"), in.errorString());
            return "";
        }

        // Store the entire file contents
        QString htmlText = in.readAll();

        in.close();
        return htmlText;
    }
}

void SaveAndLoad::SaveNotes(QWidget* parent, QString fileContent)
{
    // Opens the file explorer to select where to save the file
    fileName = QFileDialog::getSaveFileName(parent,
                                            tr("Save Notes"), "",
                                            tr("Notes (*.html)"));
    // Checks if anything was saved
    if(fileName.isEmpty()) { return; }
    else
    {
        // Checks if the file opened successfully
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::information(parent, tr("Unable to open file"),
                                             file.errorString());
            return;
        }

        // Write out the file contents
        QTextStream outFile(&file);
        outFile << fileContent;
        file.close();
    }
}

// Returns:
// Invalid = No valid file able to be opened or read
// in.readAll() = The entire .csv excel sheet data
QByteArray SaveAndLoad::LoadExcelSheet(QWidget *parent)
{
    // Allow the user to select the file through file explorer
    fileName = QFileDialog::getOpenFileName(parent,
                                            tr("Open Excel Sheet"), "",
                                            tr("Excel Sheet (*.csv)"));

    // Check if filename is empty or an invalid excel sheet
    if(fileName.isEmpty() || !CheckForValidImportSheet(parent, fileName)) { return "invalid"; }
    else
    {
        QFile in(fileName);
        // Check to make sure the file opens correctly, else throw an error
        if(!in.open(QIODevice::ReadOnly))
        {
            QMessageBox::information(parent, tr("Unable to open file"), in.errorString());
            return "invalid";
        }

        return in.readAll();
    }
}

// Checks to make sure that the user is importing the 'shopping-items' excel sheet
bool SaveAndLoad::CheckForValidImportSheet(QWidget* parent, QString sheetName)
{
    QString wikiLink = "<a href='https://github.com/Michael-R-R/GW2_Tool_Hub/wiki/Material-Tracker-Tool#error-3000'>Wiki Page</a>";

    // Correct excel sheet
    if(sheetName.contains("shopping-items"))
    {
        return true;
    }
    // Incorrect excel sheet
    else
    {
        // Create a message box with a hyperlink to the github wiki page error
        QMessageBox msgbox(parent);
        msgbox.setWindowTitle("ERROR 3000");
        msgbox.setTextFormat(Qt::RichText);
        msgbox.setText(QString("Invalid excel sheet: "
                                   "<br>Attempted to import: %1 "
                                   "<br>Imported file must be: 'shopping-items' excel sheet exported from GW2 Efficiency"
                                   "<br><br>Refer to: %2 for more information").arg(sheetName).arg(wikiLink));
        msgbox.exec();
        return false;
    }
}













