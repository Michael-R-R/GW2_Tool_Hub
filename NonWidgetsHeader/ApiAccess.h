#ifndef APIACCESS_H
#define APIACCESS_H

#include "NonWidgetsHeader/DataInterface.h"
#include "NonWidgetsHeader/ErrorHandling.h"
#include "WidgetsHeader/MaterialTracker/MaterialTracker.h"
#include <QCoreApplication>
#include <QEventLoop>
#include <QString>
#include <QDebug>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QUrl>
#include <QTime>
#include <QFile>

class MaterialTracker;

class ApiAccess : public QObject 
{

public:
    ApiAccess();
    ~ApiAccess();

    // Query API: Account Materials
    void QueryForMaterialsAPI(MaterialTracker* materialTracker);

    // Query API: Icons
    QPixmap icon;
    QPixmap QueryForMaterialIcon(QString id);

private:
    // --- Error Handling ---
    ErrorHandling error;

    // --- Data Structures ---
    QVector<QString> vMasterID;
    QVector<QString> vMasterCount;
    // Account names
    QVector<QString> vAccountNames;
    // Account inventory
    QVector<QString> vInvID;
    QVector<QString> vInvCount;
    // Account bank
    QVector<QString> vBankID;
    QVector<QString> vBankCount;

    // --- Network Access ---
    DataInterface* dataInterface;
    QNetworkAccessManager* managerAccountMaterials;

    // --- Functions ---
	void GetMaterialsApiReply(QNetworkReply* reply, MaterialTracker* materialTracker);
	void GetAccountNamesApiReply(QNetworkReply* reply, MaterialTracker* materialTracker);
	void GetInventoryApiReply(QNetworkReply* reply, MaterialTracker* materialTracker);
	void GetBankApiReply(QNetworkReply* reply, MaterialTracker* materialTracker);

	QString QueryForMaterialIconURL(QString id);
	QString GetMaterialIconUrlReply(QNetworkReply* reply);

    void delaySeconds(int secToWait);
    void delayMilliSeconds(int msecToWait);

};

#endif // APIACCESS_H
