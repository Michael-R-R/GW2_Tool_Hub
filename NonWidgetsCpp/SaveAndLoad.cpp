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
    if(fileName.isEmpty()) { return "invalid"; }
    else
    {
        // Check if we can open the file
        QFile inFile(fileName);
        if(!inFile.open(QIODevice::ReadOnly))
        {
            // Throw an error
            error.NonModalErrorMessage(parent, "Unable to open file", inFile.errorString());
            return "invalid";
        }

        // Return all the information from the file read in
        return inFile.readAll();
    }
}

// Writes out all the tracked materials in each tab to a file.
// Writes out: number of tabs, all tab names, number of materials in tab,
// all material names in tab, the material current and goal amount
// Returns: 1 = saved successfully, -1 = saved unsuccessfully, 0 = cancelled
int SaveAndLoad::SaveTrackedMaterialsToFile(QWidget* parent, int tabAmt,
                                            const QVector<QString>& tabNames,
                                            const QVector<int>& amtOfMatsInTab,
                                            const QVector<QVector<QString>>& namesInTab,
                                            const QVector<QVector<int>>& currentAmtsInTab,
                                            const QVector<QVector<int>>& goalAmtsInTab)
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
        QFile inFile(fileName);
        if(!inFile.open(QIODevice::WriteOnly))
        {
            // Throw an error
            error.NonModalErrorMessage(parent, "Unable to open file", inFile.errorString());
            return -1;
        }

        // Start writing out the data
        QTextStream outFile(&inFile);

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
        inFile.close();
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
        QFile inFile(fileName);
        if(!inFile.open(QIODevice::ReadOnly))
        {
            // Throw an error
            error.NonModalErrorMessage(parent, "Unable to open file", inFile.errorString());
            return "";
        }

        // Store the entire file contents
        QString htmlText = inFile.readAll();

        inFile.close();
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
        QFile inFile(fileName);
        if(!inFile.open(QIODevice::WriteOnly))
        {
            // Throw an error
            error.NonModalErrorMessage(parent, "Unable to open file", inFile.errorString());
            return;
        }

        // Write out the file contents
        QTextStream outFile(&inFile);
        outFile << fileContent;
        inFile.close();
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
    if(fileName.isEmpty() || !error.CheckForValidImportSheet(parent, fileName)) { return "invalid"; }
    else
    {
        QFile inFile(fileName);
        // Check to make sure the file opens correctly, else throw an error
        if(!inFile.open(QIODevice::ReadOnly))
        {
            // Throw an error
            error.NonModalErrorMessage(parent, "Unable to open file", inFile.errorString());
            return "invalid";
        }

        return inFile.readAll();
    }
}















