#ifndef DATAINTERFACE_H
#define DATAINTERFACE_H

#include <QString>
#include <QCoreApplication>
#include <QVector>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>

class DataInterface
{
public:
    DataInterface();
    ~DataInterface();

    // Getters
    int GetMaterialsCatalogSize() const {return MaterialsCatalogSize;}
    int GetMaterialsPlayerSize() const {return MaterialsPlayerSize;}
    int GetRecipesCatalogSize() const {return RecipesCatalogSize;}

    // Database: Creating tables for new tabs
    void CreateDataTableForNewTab(QString tabName);
    void DeleteTabDataTable(QString tabName);

    // Excel
    QSqlDatabase CreateExcelDatabase(QString sheetPath);
    QVector<int> FetchExcelCounts(QString sheetPath);
    QVector<QString> FetchExcelNames(QString sheetPath);

    // Database: User
    void UpdateApiKey(QString key);
    QString FetchApiKey();

    // Database: MaterialsCatalog
    QVector<QString> FetchAllMaterialCategories();
    QVector<QString> FetchAllMaterialNamesByCategory(QString category);
    QVector<QString> FetchAllMaterialNames();
    QString FetchMaterialCategory(QString materialName);
    QString FetchMaterialID(QString name);

    // Database: Tab data tables
    void UpdateTabDataTableName(QString currentTabName, QString newTabName);
    void UpdateAccountMaterialsFromAPI(QVector<QString> id, QVector<QString> count, QString tabName);
    void UpdateCurrentMaterialsAmount(QString materialName, int amount, QString tabName);
    void UpdateGoalMaterialsAmount(QString materialName, int amount, QString tabName);
    void UpdatePercentComplete(QString materialName, QString tabName);
    int FetchCurrentMaterialAmount(QString materialName, QString tabName);
    int FetchGoalMaterialAmount(QString materialName, QString tabName);
    double FetchPercentComplete(QString materialName, QString tabName);

    // Database: Recipe Catalog
    QVector<QString> FetchIngredientsID(QString recipeName);
    QVector<QString> FetchIngredientsNames(QString recipeName);
    QString FetchRecipeOutputID(QString recipeName);


private:
    // Database: Create
    QSqlDatabase userDatabase;
    QSqlDatabase CreateDatabase(QString connection, QString name, QSqlDatabase db);

    // --- Variables ---
    int MaterialsCatalogSize;
    int MaterialsPlayerSize;
    int RecipesCatalogSize;
};


#endif // DATAINTERFACE_H
