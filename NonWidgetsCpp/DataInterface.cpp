#include "NonWidgetsHeader/DataInterface.h"

DataInterface::DataInterface() :
    MaterialsCatalogSize(626), MaterialsPlayerSize(626),
    RecipesCatalogSize(12513)
{

}

DataInterface::~DataInterface()
{
    userDatabase.close();
}

/*************************************************************************
 *                        DATABASE CREATION                              *
 *************************************************************************/

// Creates a new connection to the database and opens the database
// for access. Returns the opened database
QSqlDatabase DataInterface::CreateDatabase(QString connection, QString name, QSqlDatabase db)
{
    db = QSqlDatabase::addDatabase("QSQLITE", connection);
    db.setDatabaseName(name);
    if(!db.open())
    {
        qDebug() << "ERROR: Cannot open materials database";
    }
    db.open();
    return db;
}

/*************************************************************************
 *                             Tabs                                      *
 *************************************************************************/

// Parameter: The name of the tab which sets the database table name
// Creates a table in the database for the created tab in Material Tracker
void DataInterface::CreateDataTableForNewTab(QString tabName)
{
    // Replace any spaces with no spaces
    // this ensures that the naming conventions
    // are okay for SQLite
    tabName.simplified();
    tabName.replace(" ", "");

    userDatabase = CreateDatabase("CreateNewTable", "user.db", userDatabase);
    QSqlQuery query(userDatabase);

    // CREATE THE NEW TABLE
    query.exec("CREATE TABLE IF NOT EXISTS " + tabName + " (id varchar primary key, count integer, goal integer, percentComplete double);");

    // Copy the values for MaterialsPlayer (think of MaterialsPlayer as a template)
    // and create a new table with those values
    query.exec("INSERT INTO " + tabName + " SELECT * FROM MaterialsPlayer;");

    userDatabase.removeDatabase("CreateNewTable");
}

// Parameter: The name of the tab which is used to delete the database table
// Deletes a table in the database for the created tab in Material Tracker
void DataInterface::DeleteTabDataTable(QString tabName)
{
    // Replace any spaces with no spaces
    // this ensures that the naming conventions
    // are okay for SQLite
    tabName.simplified();
    tabName.replace(" ", "");

    userDatabase = CreateDatabase("DeleteTabTable", "user.db", userDatabase);
    QSqlQuery query(userDatabase);

    // DELETE THE TABLE
    query.exec("DROP TABLE IF EXISTS " + tabName + ";");

    userDatabase.removeDatabase("DeleteTabTable");
}

/*************************************************************************
 *                             EXCEL                                     *
 *************************************************************************/

QSqlDatabase DataInterface::CreateExcelDatabase(QString sheetPath)
{
    QSqlDatabase excelDB = QSqlDatabase::addDatabase("QODBC", "xlsx_connection");
    excelDB.setDatabaseName("DRIVER={Microsoft Excel Driver (*.xls, *.xlsx, *.xlsm, *.xlsb)};"
                            "DBQ=" + sheetPath);
    excelDB.open();
    return excelDB;
}

QVector<int> DataInterface::FetchExcelCounts(QString sheetPath)
{
    QSqlDatabase excelDB = CreateExcelDatabase(sheetPath);

    // Strip down the file path to the file name
    // file name = table name
    while(sheetPath.contains("/"))
    {
        int deleteChar = 0;
        sheetPath.remove(deleteChar, 1);
    }
    if(sheetPath.contains("."))
    {
        int indexToDelete = sheetPath.indexOf(".");
        sheetPath.remove(indexToDelete, sheetPath.size());
    }
    QString tableName = sheetPath;

    // Select the material amounts
    QVector<int> matCounts;
    QSqlQuery query(excelDB);
    query.exec("SELECT Count FROM [" + tableName + "$]");
    while(query.next())
    {
        int result = query.value(0).toInt();
        matCounts.append(result);
    }

    excelDB.close();
    excelDB.removeDatabase("xlsx_connection");

    return matCounts;
}

QVector<QString> DataInterface::FetchExcelNames(QString sheetPath)
{
    QSqlDatabase excelDB = CreateExcelDatabase(sheetPath);

    // Strip down the file path to the file name
    // file name = table name
    while(sheetPath.contains("/"))
    {
        int deleteChar = 0;
        sheetPath.remove(deleteChar, 1);
    }
    if(sheetPath.contains("."))
    {
        int indexToDelete = sheetPath.indexOf(".");
        sheetPath.remove(indexToDelete, sheetPath.size());
    }
    QString tableName = sheetPath;

    // Select the material name
    QVector<QString> matNames;
    QSqlQuery query1(excelDB);
    query1.exec("SELECT Name FROM [" + tableName + "$]");
    while(query1.next())
    {
        QString result = query1.value(0).toString();
        matNames.append(result);
    }

    excelDB.close();
    excelDB.removeDatabase("xlsx_connection");

    return matNames;
}


/*************************************************************************
 *                             USER                                      *
 *************************************************************************/

// Updates the API key inside the User table database
// Called when 'File > Add API Key' is triggered
void DataInterface::UpdateApiKey(QString key)
{
    userDatabase = CreateDatabase("user", "user.db", userDatabase);

    QSqlQuery query(userDatabase);
    query.prepare("UPDATE User SET apiKey =:apiKey WHERE id =:id");
    query.bindValue(":id", "1");
    query.bindValue(":apiKey", key);
    query.exec();

    // Close connection after finish
    userDatabase.removeDatabase("user");
}

// Retrieves the API Key from User table database
// Called whenever the user queries the API network
QString DataInterface::FetchApiKey()
{
    userDatabase = CreateDatabase("user", "user.db", userDatabase);

    QSqlQuery query(userDatabase);
    query.exec("SELECT apiKey FROM User WHERE id = 1");
    query.next();
    QString key = query.value(0).toString();
    // Close connection after finish
    userDatabase.removeDatabase("user");

    return key;
}

/*************************************************************************
 *                         MATERIALS CATALOG                             *
 *************************************************************************/

// Retrieves all the material category's names and returns
// a vector of all the category names
QVector<QString> DataInterface::FetchAllMaterialCategories()
{
    userDatabase = CreateDatabase("MaterialCategories", "user.db", userDatabase);

    QVector<QString> AllCategories;
    QString result;
    QSqlQuery query(userDatabase);
    for(int i = 0; i < MaterialsCatalogSize; i++)
    {
        QCoreApplication::processEvents();

        query.prepare("SELECT categoryName FROM MaterialsCatalog WHERE rowid = ?");
        query.bindValue(0, i);
        query.exec();
        query.next();

        // Check if we have already fetched that category
        // if not, add it to the vector
        QString lastResult = result;
        result = query.value(0).toString();
        if(lastResult != result)
        {
            AllCategories.append(result);
        }
    }

    // Close connection after finish
    userDatabase.removeDatabase("MaterialCategories");

    return AllCategories;
}

// Retrieves all the material names with their corresponding category name
// returns a vector of all the material names
QVector<QString> DataInterface::FetchAllMaterialNamesByCategory(QString category)
{
    userDatabase = CreateDatabase("MaterialNamesByCategory", "user.db", userDatabase);

    QVector<QString> AllTypes;
    QSqlQuery query(userDatabase);
    query.prepare("SELECT name FROM MaterialsCatalog WHERE categoryName = ?");
    query.bindValue(0, category);
    query.exec();
    while(query.next())
    {
        QCoreApplication::processEvents();

        QString result = query.value(0).toString();
        AllTypes.append(result);
    }

    // Close connection after finish
    userDatabase.removeDatabase("MaterialNamesByCategory");

    return AllTypes;
}

// Retrieves all the material names in the MaterialsCatalog table
// and returns a vector of all the names
QVector<QString> DataInterface::FetchAllMaterialNames()
{
    userDatabase = CreateDatabase("AllMaterialNames", "user.db", userDatabase);

    QVector<QString> AllTypes;
    QSqlQuery query(userDatabase);
    query.prepare("SELECT name FROM MaterialsCatalog WHERE rowid > ?");
    query.bindValue(0, 0);
    query.exec();
    while(query.next())
    {
        QCoreApplication::processEvents();

        QString result = query.value(0).toString();
        AllTypes.append(result);
    }

    // Close connection after finish
    userDatabase.removeDatabase("AllMaterialNames");

    return AllTypes;
}

// Searches for the category name from the passed in material name
// and returns the category name
QString DataInterface::FetchMaterialCategory(QString materialName)
{
    userDatabase = CreateDatabase("FetchCategory", "user.db", userDatabase);

    QSqlQuery query(userDatabase);
    query.prepare("SELECT categoryName FROM MaterialsCatalog WHERE name = ?");
    query.bindValue(0, materialName);
    query.exec();
    query.next();
    QString result = query.value(0).toString();

    // Close connection after finish
    userDatabase.removeDatabase("FetchCategory");

    return result;
}

// Retreieves the material ID of the passed in material name
// from the MaterialsCatalog
QString DataInterface::FetchMaterialID(QString name)
{
    userDatabase = CreateDatabase("MaterialsCatalogID", "user.db", userDatabase);

    QSqlQuery query(userDatabase);
    query.prepare("SELECT id FROM MaterialsCatalog WHERE name = ?");
    query.bindValue(0, name);
    query.exec();
    query.next();
    QString result = query.value(0).toString();
    // Close connection after finish
    userDatabase.removeDatabase("MaterialsCatalogID");

    return result;
}

/*************************************************************************
 *                          TAB DATA TABLES                              *
 *************************************************************************/

// Renames the corresponding data table with the new passed in tab name
void DataInterface::UpdateTabDataTableName(QString currentTabName, QString newTabName)
{

    // Replace any spaces with no spaces
    // this ensures that the naming conventions
    // are okay for SQLite
    currentTabName.simplified();
    currentTabName.replace(" ", "");

    newTabName.simplified();
    newTabName.replace(" ", "");

    userDatabase = CreateDatabase("UpdateTabName", "user.db", userDatabase);

    QSqlQuery query(userDatabase);
    query.exec("ALTER TABLE " + currentTabName + " RENAME TO " + newTabName);

    userDatabase.removeDatabase("UpdateTabName");
    qDebug() << "Tab name updated";
}

// Updates all the material count amounts inside each
// tab database table.
void DataInterface::UpdateAccountMaterialsFromAPI(const QVector<QString>& id,
                                                  const QVector<QString>& count, QString tabName)
{
    // Replace any spaces with no spaces
    // this ensures that the naming conventions
    // are okay for SQLite
    tabName.simplified();
    tabName.replace(" ", "");

    userDatabase = CreateDatabase("MaterialsUpdateCurrentAmount", "user.db", userDatabase);

    // Matches the passed in material ID and sets its count amount
    QSqlQuery query(userDatabase);
    query.prepare("UPDATE " + tabName + " SET count =:count WHERE id =:id");
    for(int i = 0; i < id.size(); i++)
    {
        QCoreApplication::processEvents();

        query.bindValue(":count", count[i].toInt());
        query.bindValue(":id", id[i]);
        query.exec();
    }

    // Close connection after finish
    userDatabase.removeDatabase("MaterialsUpdateCurrentAmount");
    qDebug() << "Current amount updated";
}

// Updates the current amount in the database for the
// passed in material name and amount
void DataInterface::UpdateCurrentMaterialsAmount(QString materialName, int amount, QString tabName)
{
    // Replace any spaces with no spaces
    // this ensures that the naming conventions
    // are okay for SQLite
    tabName.simplified();
    tabName.replace(" ", "");

    // Fetch the material id from the MaterialsCatalog with the material name
    QString materialID = FetchMaterialID(materialName);

    // Update current amount
    userDatabase = CreateDatabase("UpdateCurrentAmount", "user.db", userDatabase);
    QSqlQuery query(userDatabase);
    query.prepare("UPDATE " + tabName + " SET count =:count WHERE id =:id");
    query.bindValue(":count", amount);
    query.bindValue(":id", materialID);
    query.exec();
    userDatabase.removeDatabase("UpdateCurrentAmount");
}

// Updates the goal amount in the database for the
// passed in material name and amount
void DataInterface::UpdateGoalMaterialsAmount(QString materialName, int amount, QString tabName)
{
    tabName.simplified();
    tabName.replace(" ", "");

    // Fetch the material id from the MaterialsCatalog with the material name
    QString materialID = FetchMaterialID(materialName);

    // Update current amount
    userDatabase = CreateDatabase("UpdateGoalAmount", "user.db", userDatabase);
    QSqlQuery query(userDatabase);
    query.prepare("UPDATE " + tabName + " SET goal =:goal WHERE id =:id");
    query.bindValue(":goal", amount);
    query.bindValue(":id", materialID);
    query.exec();
    userDatabase.removeDatabase("UpdateGoalAmount");
}

// Updates the tracked materials percent complete in the database
// for the passed in material name and percent
void DataInterface::UpdatePercentComplete(QString materialName, QString tabName)
{
    // Replace any spaces with no spaces
    // this ensures that the naming conventions
    // are okay for SQLite
    tabName.simplified();
    tabName.replace(" ", "");

    // Fetch the material id from the MaterialsCatalog with the material name
    QString materialID = FetchMaterialID(materialName);

    // Calculate the percent complete = (current/goal) * 100
    int currentAmount = FetchCurrentMaterialAmount(materialName, tabName);
    int goalAmount = FetchGoalMaterialAmount(materialName, tabName);
    double goalConverted = double(goalAmount);
    double percent = (currentAmount / goalConverted) * 100;

    // Update current amount
    userDatabase = CreateDatabase("UpdateGoalAmount", "user.db", userDatabase);
    QSqlQuery query(userDatabase);
    query.prepare("UPDATE " + tabName + " SET percentComplete =:percent WHERE id =:id");
    query.bindValue(":percent", percent);
    query.bindValue(":id", materialID);
    query.exec();
    userDatabase.removeDatabase("UpdateGoalAmount");
}

// Retrieves the material count amount for the passed in
// material ID code from the MaterialsAPI table database.
int DataInterface::FetchCurrentMaterialAmount(QString materialName, QString tabName)
{
    // Replace any spaces with no spaces
    // this ensures that the naming conventions
    // are okay for SQLite
    tabName.simplified();
    tabName.replace(" ", "");

    // Fetch material id with material name
    QString materialID = FetchMaterialID(materialName);

    // Fetch the material amount from MaterialsPlayer with the material id
    userDatabase = CreateDatabase("MaterialsCurrentAmount", "user.db", userDatabase);
    QSqlQuery query2(userDatabase);
    query2.prepare("SELECT count FROM " + tabName + " WHERE id = ?");
    query2.bindValue(0, materialID);
    query2.exec();
    query2.next();
    int result = query2.value(0).toInt();
    userDatabase.removeDatabase("MaterialsCurrentAmount");

    return result;
}

// Retrieves the goal amount for the given passed in material
int DataInterface::FetchGoalMaterialAmount(QString materialName, QString tabName)
{
    // Replace any spaces with no spaces
    // this ensures that the naming conventions
    // are okay for SQLite
    tabName.simplified();
    tabName.replace(" ", "");

    // Fetch material id with material name
    QString materialID = FetchMaterialID(materialName);

    // Fetch the material amount
    userDatabase = CreateDatabase("MaterialsGoalAmount", "user.db", userDatabase);
    QSqlQuery query2(userDatabase);
    query2.prepare("SELECT goal FROM " + tabName + " WHERE id = ?");
    query2.bindValue(0, materialID);
    query2.exec();
    query2.next();
    int result = query2.value(0).toInt();
    userDatabase.removeDatabase("MaterialsGoalAmount");

    return result;
}

// Retrieves the percent complete for the given passed in material
double DataInterface::FetchPercentComplete(QString materialName, QString tabName)
{
    // Replace any spaces with no spaces
    // this ensures that the naming conventions
    // are okay for SQLite
    tabName.simplified();
    tabName.replace(" ", "");

    // Fetch material id with material name
    QString materialID = FetchMaterialID(materialName);

    // Fetch the material amount
    userDatabase = CreateDatabase("MaterialsPercentComplete", "user.db", userDatabase);
    QSqlQuery query2(userDatabase);
    query2.prepare("SELECT percentComplete FROM " + tabName + " WHERE id = ?");
    query2.bindValue(0, materialID);
    query2.exec();
    query2.next();
    double result = query2.value(0).toDouble();
    userDatabase.removeDatabase("MaterialsPercentComplete");

    return result;
}

/*************************************************************************
 *                          RECIPE CATALOG                               *
 *************************************************************************/

// Retrieves all the ingredient's IDs for the requested recipe
// Returns: vector of all the IDs
QVector<QString> DataInterface::FetchIngredientsID(QString recipeName)
{
    userDatabase = CreateDatabase("FetchIngredientsID", "user.db", userDatabase);

    QSqlQuery query(userDatabase);
    query.prepare("SELECT ingredientsID FROM RecipesCatalog WHERE recipeName = ?");
    query.bindValue(0, recipeName);
    query.exec();
    query.first();
    QString result = query.value(0).toString();
    // Close connection after finish
    userDatabase.removeDatabase("FetchIngredientsID");

    // Removes the new line from the fetch result
    // and adds the edited ID to a vector in case there
    // are multiple ingredients
    QVector<QString> AllIngredientsID;
    int newLineIndex = 0;
    while(result.contains("\n"))
    {
        QString id;
        newLineIndex = result.indexOf("\n");
        for(int i = 0; i < newLineIndex; i++)
        {
            id += result[i];
        }
        AllIngredientsID.append(id);
        result.remove(0, newLineIndex + 1);
    }

    return AllIngredientsID;
}

// Retrieves all the ingredient's names for the requested recipe
// Returns: vector of all the names
QVector<QString> DataInterface::FetchIngredientsNames(QString recipeName)
{
    userDatabase = CreateDatabase("FetchingIngredientsNames", "user.db", userDatabase);

    QSqlQuery query(userDatabase);
    query.prepare("SELECT ingredientsName FROM RecipesCatalog WHERE recipeName = ?");
    query.bindValue(0, recipeName);
    query.exec();
    query.first();
    QString result = query.value(0).toString();
    qDebug() << "RecipesCatalog Result: " << result;

    userDatabase.removeDatabase("FetchingIngredientsNames");

    // Removes the new line from the fetch result
    // and adds the edited name to a vector in case there
    // are multiple ingredients
    QVector<QString> AllIngredientNames;
    int newLineIndex = 0;
    while(result.contains("\n"))
    {
        QString name;
        newLineIndex = result.indexOf("\n");
        for(int i = 0; i < newLineIndex; i++)
        {
            name += result[i];
        }
        AllIngredientNames.append(name);
        result.remove(0, newLineIndex + 1);
    }

    return AllIngredientNames;
}

// Retrieves the recipe outputID for the request recipe
// outputID = the recipe itself (I.E. 29169 = Dawn)
QString DataInterface::FetchRecipeOutputID(QString recipeName)
{
    userDatabase = CreateDatabase("FetchOutputID", "user.db", userDatabase);

    QSqlQuery query(userDatabase);
    query.prepare("SELECT outputItemID FROM RecipesCatalog WHERE recipeName = ?");
    query.bindValue(0, recipeName);
    query.exec();
    query.first();
    QString result = query.value(0).toString();
    // Close connection after finish
    userDatabase.removeDatabase("FetchOutputID");

    return result;
}






















