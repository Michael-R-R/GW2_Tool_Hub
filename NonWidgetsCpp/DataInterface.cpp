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
    query.exec("CREATE TABLE IF NOT EXISTS " + tabName + " (id varchar primary key, count integer, goal integer, percentComplete double, icon BLOB);");

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
    QVector<int> vMatCounts;
    QSqlQuery query(excelDB);
    query.exec("SELECT Count FROM [" + tableName + "$]");
    while(query.next())
    {
        int result = query.value(0).toInt();
        vMatCounts.append(result);
    }

    excelDB.close();
    excelDB.removeDatabase("xlsx_connection");

    return vMatCounts;
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
    QVector<QString> vMatNames;
    QSqlQuery query1(excelDB);
    query1.exec("SELECT Name FROM [" + tableName + "$]");
    while(query1.next())
    {
        QString result = query1.value(0).toString();
        vMatNames.append(result);
    }

    excelDB.close();
    excelDB.removeDatabase("xlsx_connection");

    return vMatNames;
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

QVector<QString> DataInterface::FetchAllMaterialIds()
{
	userDatabase = CreateDatabase("MaterialIds", "user.db", userDatabase);

    QVector<QString> vID;
    QString result;
    QSqlQuery query(userDatabase);
    for (int i = 1; i <= MaterialsCatalogSize; i++)
    {
        QCoreApplication::processEvents();

        query.prepare("SELECT id FROM MaterialsCatalog WHERE rowid = ?");
        query.bindValue(0, i);
        query.exec();
        query.next();

        // Add id to vector
        result = query.value(0).toString();
        vID.append(result);
    }

	// Close connection after finish
	userDatabase.removeDatabase("MaterialCategories");

	return vID;
}

// Retrieves all the material category's names and returns
// a vector of all the category names
QVector<QString> DataInterface::FetchAllMaterialCategories()
{
    userDatabase = CreateDatabase("MaterialCategories", "user.db", userDatabase);

    QVector<QString> vAllCategories;
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
            vAllCategories.append(result);
        }
    }

    // Close connection after finish
    userDatabase.removeDatabase("MaterialCategories");

    return vAllCategories;
}

// Retrieves all the material names with their corresponding category name
// returns a vector of all the material names
QVector<QString> DataInterface::FetchAllMaterialNamesByCategory(QString category)
{
    userDatabase = CreateDatabase("MaterialNamesByCategory", "user.db", userDatabase);

    QVector<QString> vAllTypes;
    QSqlQuery query(userDatabase);
    query.prepare("SELECT name FROM MaterialsCatalog WHERE categoryName = ?");
    query.bindValue(0, category);
    query.exec();
    while(query.next())
    {
        QCoreApplication::processEvents();

        QString result = query.value(0).toString();
        vAllTypes.append(result);
    }

    // Close connection after finish
    userDatabase.removeDatabase("MaterialNamesByCategory");

    return vAllTypes;
}

// Retrieves all the material names in the MaterialsCatalog table
// and returns a vector of all the names
QVector<QString> DataInterface::FetchAllMaterialNames()
{
    userDatabase = CreateDatabase("AllMaterialNames", "user.db", userDatabase);

    QVector<QString> vAllNames;
    QSqlQuery query(userDatabase);
    query.prepare("SELECT name FROM MaterialsCatalog WHERE rowid > ?");
    query.bindValue(0, 0);
    query.exec();
    while(query.next())
    {
        QCoreApplication::processEvents();

        QString result = query.value(0).toString();
        vAllNames.append(result);
    }

    // Close connection after finish
    userDatabase.removeDatabase("AllMaterialNames");

    return vAllNames;
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

// Retrieves the material ID of the passed in material name
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

void DataInterface::UpdateMaterialIcons(int id, QPixmap icon)
{
    userDatabase = CreateDatabase("UpdateMaterialsIcon", "user.db", userDatabase);

    // Write QPixmap into QByteArray in .png format
    QByteArray inByteArray;
    QBuffer inBuffer(&inByteArray);
    inBuffer.open(QIODevice::WriteOnly); 
    icon.save(&inBuffer, "PNG");

    QSqlQuery query(userDatabase);
    query.prepare("UPDATE MaterialsCatalog SET icon =:icon WHERE id =:id");
    query.bindValue(":icon", inByteArray);
    query.bindValue(":id", id);
    query.exec();

    userDatabase.removeDatabase("UpdateMaterialsIcon");
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
    QSqlQuery query(userDatabase);
    query.prepare("SELECT count FROM " + tabName + " WHERE id = ?");
    query.bindValue(0, materialID);
    query.exec();
    query.next();
    int result = query.value(0).toInt();
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
    QSqlQuery query(userDatabase);
    query.prepare("SELECT goal FROM " + tabName + " WHERE id = ?");
    query.bindValue(0, materialID);
    query.exec();
    query.next();
    int result = query.value(0).toInt();
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
    QSqlQuery query(userDatabase);
    query.prepare("SELECT percentComplete FROM " + tabName + " WHERE id = ?");
    query.bindValue(0, materialID);
    query.exec();
    query.next();
    double result = query.value(0).toDouble();
    userDatabase.removeDatabase("MaterialsPercentComplete");

    return result;
}

// Retrieves the material icon for the given passed in material
QByteArray DataInterface::FetchMaterialIcon(QString materialName, QString tabName)
{
	// Replace any spaces with no spaces
	// this ensures that the naming conventions
	// are okay for SQLite
	tabName.simplified();
	tabName.replace(" ", "");

    QString materialID = FetchMaterialID(materialName);

    // Fetch the material icon
    userDatabase = CreateDatabase("FetchMaterialIcon", "user.db", userDatabase);
    QSqlQuery query(userDatabase);
    query.prepare("SELECT icon FROM " + tabName + " WHERE id = ?");
    query.bindValue(0, materialID);
    query.exec();
    query.next();
    QByteArray outByteArray = query.value(0).toByteArray();
	userDatabase.removeDatabase("FetchMaterialIcon");

	return outByteArray;
    
}

/*************************************************************************
 *                          RECIPE CATALOG                               *
 *************************************************************************/

void DataInterface::UpdateRecipeCatalogTable(QString recipeID, QString itemID,
                                             QString ingredName, QString count)
{
    userDatabase = CreateDatabase("UpdateRecipeCatalogTable", "user.db", userDatabase);

    QSqlQuery queryItemID(userDatabase);
    queryItemID.prepare("UPDATE RecipesCatalog SET ingredientsID =:itemID WHERE id =:recipeID");
    queryItemID.bindValue(":itemID", itemID);
    queryItemID.bindValue(":recipeID", recipeID);
    queryItemID.exec();

    QSqlQuery queryIngredName(userDatabase);
    queryIngredName.prepare("UPDATE RecipesCatalog SET ingredientsName =:ingredName WHERE id =:recipeID");
    queryIngredName.bindValue(":ingredName", ingredName);
    queryIngredName.bindValue(":recipeID", recipeID);
    queryIngredName.exec();

    QSqlQuery queryCount(userDatabase);
    queryCount.prepare("UPDATE RecipesCatalog SET ingredientAmountRequired =:count WHERE id =:recipeID");
    queryCount.bindValue(":count", count);
    queryCount.bindValue(":recipeID", recipeID);
    queryCount.exec();

    userDatabase.removeDatabase("UpdateRecipeCatalogTable");
}


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
    QVector<QString> vAllIngredientsID;
    QTextStream text(&result);
    while (!text.atEnd())
    {
        QString line = text.readLine();

        vAllIngredientsID.append(line);
    }

    return vAllIngredientsID;
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

    userDatabase.removeDatabase("FetchingIngredientsNames");

    QVector<QString> vAllIngredientNames;
    QTextStream text(&result);
    while(!text.atEnd())
    {
        QString line = text.readLine();
        
        vAllIngredientNames.append(line);
    }

    return vAllIngredientNames;
}

QVector<QString> DataInterface::FetchIngredientsCount(QString recipeName)
{
    userDatabase = CreateDatabase("FetchIngredientCounts", "user.db", userDatabase);

    QSqlQuery query(userDatabase);
    query.prepare("SELECT ingredientAmountRequired FROM RecipesCatalog WHERE recipeName = ?");
    query.bindValue(0, recipeName);
    query.exec();
    query.first();
    QString result = query.value(0).toString();

    userDatabase.removeDatabase("FetchIngredientCounts");

    QVector<QString> vAllIngredientCounts;
    QTextStream text(&result);
    while (!text.atEnd())
    {
        QString line = text.readLine();

        vAllIngredientCounts.append(line);
    }

    return vAllIngredientCounts;
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

QVector<QString> DataInterface::FetchAllRecipeNames()
{
    userDatabase = CreateDatabase("AllRecipeNames", "user.db", userDatabase);

    QVector<QString> vRecipeNames;

    QSqlQuery query(userDatabase);
    query.prepare("SELECT recipeName FROM RecipesCatalog WHERE rowid > ?");
    query.bindValue(0, 0);
    query.exec();
    while (query.next())
    {
        QCoreApplication::processEvents();

        QString result = query.value(0).toString();
        vRecipeNames.append(result);
    }

    // Close connection after finish
    userDatabase.removeDatabase("AllRecipeNames");

    return vRecipeNames;
}

QVector<QString> DataInterface::FetchAllRecipeIDs()
{
    userDatabase = CreateDatabase("FetchAllRecipeIDs", "user.db", userDatabase);

    QVector<QString> vAllRecipeNames;

    QSqlQuery query(userDatabase);
    query.prepare("SELECT id FROM RecipesCatalog WHERE rowid > ?");
    query.bindValue(0, 0);
    query.exec();
    while (query.next())
    {
        QCoreApplication::processEvents();

        QString result = query.value(0).toString();
        vAllRecipeNames.append(result);
    }

    userDatabase.removeDatabase("FetchAllRecipeIDs");

    return vAllRecipeNames;
}

bool DataInterface::CheckForValidRecipe(QString recipeName)
{
    userDatabase = CreateDatabase("CheckValidRecipeName", "user.db", userDatabase);

    QSqlQuery query(userDatabase);
    query.prepare("SELECT id FROM RecipesCatalog WHERE recipeName = ?");
    query.bindValue(0, recipeName);
    query.exec();
    query.first();
    QString result = query.value(0).toString();

    userDatabase.removeDatabase("CheckValidRecipeName");

    if (result.isEmpty()) return false;
    else return true;
}

/*************************************************************************
 *                         DEVELOPER FUNCTIONS                           *
 *************************************************************************/

void DataInterface::CopyMasterMaterialsCatalog()
{
	userDatabase = CreateDatabase("CopyMasterCatalog", "user.db", userDatabase);
	QSqlQuery query(userDatabase);

	// Copy the values from MaterialsCatalog to MaterialsPlayer
	// Think of MaterialsPlayer as a template data table
    for (int i = 1; i < MaterialsCatalogSize; i++)
    {
        query.exec("UPDATE MaterialsPlayer SET icon=(SELECT icon FROM MaterialsCatalog WHERE MaterialsCatalog.id = MaterialsPlayer.id)");
        query.next();
        qDebug() << "Inserting icon";
    }

	userDatabase.removeDatabase("CopyMasterCatalog");
}



















