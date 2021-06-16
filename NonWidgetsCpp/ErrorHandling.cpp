#include "NonWidgetsHeader/ErrorHandling.h"

ErrorHandling::ErrorHandling()
{

}

ErrorHandling::~ErrorHandling()
{

}

/*************************************************************************
 *                           POPUP MESSAGE BOXES                         *
 *************************************************************************/

// A popup messagebox that is nonmodal and outputs the error message
// meaning it won't block the flow of code execution
void ErrorHandling::NonModalErrorMessage(QWidget* parent, QString title, QString text)
{
    QMessageBox* msgbox = new QMessageBox(parent);
    msgbox->setWindowModality(Qt::NonModal);
    msgbox->setAttribute(Qt::WA_DeleteOnClose);
    msgbox->setStandardButtons(QMessageBox::Ok);
    msgbox->setWindowTitle(title);
    msgbox->setText(text);
    msgbox->show();
}

/*************************************************************************
 *                            MATERIAL TRACKER                           *
 *************************************************************************/

// Error 3001
// Checks if the tab name that the user entered is valid.
// There are certain checks that must be made in order
// for the user entered tab name to be reflected in the
// SQLite database.
bool ErrorHandling::CheckForValidTabName(QWidget *parent, QString name, const QVector<QString>& activeNames)
{
    if(name.contains("-") || name.contains("."))
    {
        // Display message box with hyperlink to the wiki for the given Error code
        QString wikiLink = "<a href='https://github.com/Michael-R-R/GW2_Tool_Hub/wiki/Material-Tracker-Tool#error-3001-invalid-tab-name'>Wiki Page</a>";
        QString title = "Error 3001";
        QString text = QString("Invalid tab name: "
                               "<br>Attempted to name tab: %1 "
                               "<br>Tab name cannot contain '-' or '.'"
                               "<br><br>Refer to: %2 for more information").arg(name, wikiLink);

        HyperlinkErrorMessage(parent, title, text);
        return false;
    }

    // Check if name starts with a number
    QString numbers = "0123456789";
    for(int i = 0; i < numbers.size(); i++)
    {
        if(name[0] == numbers[i])
        {
            // Display message box with hyperlink to the wiki for the given Error code
            QString wikiLink = "<a href='https://github.com/Michael-R-R/GW2_Tool_Hub/wiki/Material-Tracker-Tool#error-3001-invalid-tab-name'>Wiki Page</a>";
            QString title = "Error 3001";
            QString text = QString("Invalid tab name: "
                                   "<br>Attempted to name tab: %1 "
                                   "<br>Tab name cannot begin with a number"
                                   "<br><br>Refer to: %2 for more information").arg(name, wikiLink);

            HyperlinkErrorMessage(parent, title, text);
            return false;
        }
    }

    // Check for existing tab names
    for(int i = 0; i < activeNames.size(); i++)
    {
        // Strip the new tab name and the current tab name
        // down to what SQLite sees to make sure that
        // there are no conflicting names
        QString tempNewName = name.simplified();
        tempNewName.replace(" ", "");

        QString tempCurrentName = activeNames[i].simplified();
        tempCurrentName.replace(" ", "");

        // Names match, invalid name
        if(tempNewName.toLower() == tempCurrentName.toLower())
        {
            // Display message box with hyperlink to the wiki for the given Error code
            QString wikiLink = "<a href='https://github.com/Michael-R-R/GW2_Tool_Hub/wiki/Material-Tracker-Tool#error-3001-invalid-tab-name'>Wiki Page</a>";
            QString title = "Error 3001";
            QString text = QString("Invalid tab name: "
                                   "<br>Attempted to name tab: %1 "
                                   "<br>Tab name already exists"
                                   "<br><br>Refer to: %2 for more information").arg(name, wikiLink);

            HyperlinkErrorMessage(parent, title, text);
            return false;
        }
    }

    // Name is valid
    return true;
}


// Error #3000
// Checks to make sure that the user is importing the 'shopping-items' excel sheet
bool ErrorHandling::CheckForValidImportSheet(QWidget* parent, QString sheetName)
{
    // Correct excel sheet
    if(sheetName.contains("shopping-items"))
    {
        return true;
    }
    // Incorrect excel sheet
    else
    {
        // Display message box with hyperlink to the wiki for the given Error code
        QString wikiLink = "<a href='https://github.com/Michael-R-R/GW2_Tool_Hub/wiki/Material-Tracker-Tool#error-3000'>Wiki Page</a>";
        QString title = "Error 3000";
        QString text = QString("Invalid excel sheet: "
                               "<br>Attempted to import: %1 "
                               "<br>Imported file must be: 'shopping-items' excel sheet exported from GW2 Efficiency"
                               "<br><br>Refer to: %2 for more information").arg(sheetName, wikiLink);

        HyperlinkErrorMessage(parent, title, text);
        return false;
    }
}

/*************************************************************************
 *                             BROWSER                                   *
 *************************************************************************/

// Function to check if the user is going to a valid website address
// Returns true if valid, false if not
bool ErrorHandling::CheckForValidPage(QWidget* parent, const QUrl &url)
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
    NonModalErrorMessage(parent, "Error", "Invalid Page");
    return false;
}

/*************************************************************************
 *                           PRIVATE FUNCTIONS                           *
 *************************************************************************/

// Contains a modal popup messagebox that allows for hyperlinks
// to be embebbed in the text
void ErrorHandling::HyperlinkErrorMessage(QWidget *parent, QString title, QString text)
{
    QMessageBox msgbox(parent);
    msgbox.setWindowTitle(title);
    msgbox.setTextFormat(Qt::RichText);
    msgbox.setText(text);
    msgbox.exec();
}
