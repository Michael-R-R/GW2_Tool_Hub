#include "NonWidgetsHeader/ApiAccess.h"

ApiAccess::ApiAccess() :
    vMasterID(), vMasterCount(), vAccountNames(),
    vInvID(), vInvCount(), vBankID(), vBankCount(),
    dataInterface(new DataInterface),
    managerAccountMaterials(new QNetworkAccessManager)
{

}

ApiAccess::~ApiAccess()
{
    delete managerAccountMaterials;
    delete dataInterface;
}

/*************************************************************************
 *                          PUBLIC FUNCTIONS                             *
 *************************************************************************/

// Sends a request for the users material API information
// Called: When 'Update Materials' push button is pressed
void ApiAccess::QueryForMaterialsAPI(MaterialTracker* materialTracker)
{
    // QUERY PLAYERS MATERIAL STORAGE
    {
        // Fetch the users API key from the database for authentication
        QUrl url("https://api.guildwars2.com/v2/account/materials?access_token=" + dataInterface->FetchApiKey());
        QNetworkRequest request(url);
        QNetworkReply *storageReply = managerAccountMaterials->get(request);

        // Loop until we get the reply back
        // Ensures the application doesn't freeze while waiting
        QEventLoop loop;
        connect(storageReply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();
        // Process the reply and parses all required data
        GetMaterialsApiReply(storageReply, materialTracker);
        storageReply->deleteLater();
    }

    // QUERY FOR ALL PLAYERS ACCOUNTS NAMES
    {
        QUrl url("https://api.guildwars2.com/v2/characters?access_token=" + dataInterface->FetchApiKey());
        QNetworkRequest request(url);
        QNetworkReply *namesReply = managerAccountMaterials->get(request);

        // Loop until we get the reply back
        // Ensures the application doesn't freeze while waiting
        QEventLoop loop;
        connect(namesReply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();
        // Process the reply
        GetAccountNamesApiReply(namesReply, materialTracker);
        namesReply->deleteLater();
    }

    // QUERY EACH ACCOUNTS INVENTORY
    {
        for(auto name : vAccountNames)
        {
            QUrl url("https://api.guildwars2.com/v2/characters/" + name + "/inventory?access_token=" + dataInterface->FetchApiKey());
            QNetworkRequest request(url);
            QNetworkReply *invReply = managerAccountMaterials->get(request);

            // Loop until we get the reply back
            // Ensures the application doesn't freeze while waiting
            QEventLoop loop;
            connect(invReply, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();
            // Process the reply
            GetInventoryApiReply(invReply, materialTracker);
            invReply->deleteLater();
        }
    }

    // QUERY THE PLAYERS BANK
    {
        QUrl url("https://api.guildwars2.com/v2/account/bank?access_token=" + dataInterface->FetchApiKey());
        QNetworkRequest request(url);
        QNetworkReply *bankReply = managerAccountMaterials->get(request);

        // Loop until we get the reply back
        // Ensures the application doesn't freeze while waiting
        QEventLoop loop;
        connect(bankReply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();
        // Process the reply
        GetBankApiReply(bankReply, materialTracker);
        bankReply->deleteLater();
    }

    // COMPARE MATERIALS AND ADD EACH MATCHED VECTOR AMOUNT TOGETHER
    // THEN STORE IN THE MASTER VECTOR
    {
        // COMPARE ALL INVENTORY TO MASTER AND ADD FOUND MATCHES
        for(int i = 0; i < vInvID.size(); i++)
        {
            // Update the ui Material Tracker status label
            materialTracker->ChangeWaitingForApiReplyStatusLabel();

            // Checks if IDs match, gets index of the master of the matching ID,
            // retrieves the master count, retrieves the inventory count, and
            // updates the new master count
            if(vMasterID.contains(vInvID[i]))
            {
                int indexHit = vMasterID.indexOf(vInvID[i]);
                int masterCount = vMasterCount[indexHit].toInt();
                int invCount = vInvCount[i].toInt();
                int addedAmount = invCount + masterCount;
                // Update total amount
                vMasterCount[indexHit] = QString::number(addedAmount);
            }
        }

        // COMPARE BANK TO MASTER AND ADD FOUND MATCHES
        for(int i = 0; i < vBankID.size(); i++)
        {
            // Update the ui Material Tracker status label
            materialTracker->ChangeWaitingForApiReplyStatusLabel();

            // Checks if IDs match, gets index of the master of the matching ID,
            // retrieves the master count, retrieves the bank count, and
            // updates the new master count
            if(vMasterID.contains(vBankID[i]))
            {
                int indexHit = vMasterID.indexOf(vBankID[i]);
                int masterCount = vMasterCount[indexHit].toInt();
                int bankCount = vBankCount[i].toInt();
                int addedAmount = bankCount + masterCount;
                vMasterCount[indexHit] = QString::number(addedAmount);
            }
        }
    }

    // Fill in all active tab's table database
    // with the material API data
    QVector<QString> tempName = materialTracker->GetAllTabNames();
    for(auto tabName : tempName)
    {
        dataInterface->UpdateAccountMaterialsFromAPI(vMasterID, vMasterCount, tabName);
    }
}

QPixmap ApiAccess::QueryForMaterialIcon(QString id)
{
    QUrl url = QUrl(QueryForMaterialIconURL(id));
    QNetworkAccessManager manager;
	QEventLoop loop;
	connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
	QNetworkReply* reply = manager.get(QNetworkRequest(url));
	loop.exec();
	reply->deleteLater();

	icon.loadFromData(reply->readAll());
	return icon;
}

/*************************************************************************
 *                         PRIVATE FUNCTIONS                             *
 *************************************************************************/

 // Receives reply from API
 // Parses the id and count information and stores inside master vectors
void ApiAccess::GetMaterialsApiReply(QNetworkReply* reply, MaterialTracker* materialTracker)
{
	// Error getting reply from query
	if (reply->error())
	{
		// Display the error to the user with a popup message
		error.NonModalErrorMessage(materialTracker, "Materials API Reply Error", reply->errorString());
		return;
	}

	// API answer
	QString answer = reply->readAll();

	// Parse the reply and add the information to
	// each corresponding vector
	QString id;
	QString count;
	QTextStream text(&answer);
	while (!text.atEnd())
	{
		// Update the ui material tracker status label
		materialTracker->ChangeWaitingForApiReplyStatusLabel();

		QString line = text.readLine();

		// Parse the material id
		if (line.contains("\"id\":"))
		{
			for (int i = line.indexOf(":") + 2; i < line.size() - 1; i++)
			{
				id += line[i];
			}
		}
		// Parse the material count
		if (line.contains("\"count\":"))
		{
			for (int i = line.indexOf(":") + 2; i < line.size(); i++)
			{
				count += line[i];
			}
			// Append ID and count at same time to avoid appending
			// the wrong ID, since there can be multiple in the reply
			vMasterID.append(id);
			vMasterCount.append(count);
			// clear the strings for the next values
			id.clear();
			count.clear();
		}
	}
}

// Receives reply from API
// Parses the account names and stores inside vector
void ApiAccess::GetAccountNamesApiReply(QNetworkReply* reply, MaterialTracker* materialTracker)
{
	// Error getting reply from query
	if (reply->error())
	{
		// Display the error to the user with a popup message
		error.NonModalErrorMessage(materialTracker, "Account Names API Reply error", reply->errorString());
		return;
	}

	// API answer
	QString answer = reply->readAll();

	// Parse the reply for the account names
	// and add them to a vector
	QTextStream text(&answer);
	while (!text.atEnd())
	{
		// Update the ui material tracker status label
		materialTracker->ChangeWaitingForApiReplyStatusLabel();

		// Read one line at a time from the reply
		QString line = text.readLine();
		// Parse for all names
		// check if line contains a quotation mark
		if (line.contains("\""))
		{
			QString name;
			for (int i = line.indexOf("\"") + 1; i < line.lastIndexOf("\""); i++)
			{
				name += line[i];
			}
			vAccountNames.append(name);
		}
	}
}

// Receives reply from API
// Parses characters inventory for
// material id and count amount
void ApiAccess::GetInventoryApiReply(QNetworkReply* reply, MaterialTracker* materialTracker)
{
	// Error getting reply from query
	if (reply->error())
	{
		// Display the error to the user with a popup message
		error.NonModalErrorMessage(materialTracker, "Inventory API Reply Error", reply->errorString());
		return;
	}

	// API answer
	QString answer = reply->readAll();

	// Parse the reply
	QString tempCount;
	QString tempID;
	QTextStream text(&answer);
	while (!text.atEnd())
	{
		// Update the ui material tracker status label
		materialTracker->ChangeWaitingForApiReplyStatusLabel();

		// Read one line at a time
		QString line = text.readLine();

		// Parse the material id
		if (line.contains("\"id\":"))
		{
			// Clear the ID string before reading
			// a new one because there can be multiple IDs
			// in the reply and we only want the material IDs
			tempID.clear();
			for (int i = line.indexOf(":") + 2; i < line.size() - 1; i++)
			{
				tempID += line[i];
			}
		}
		// Parse the material count
		if (line.contains("\"count\":"))
		{
			if (line.contains(","))
			{
				for (int i = line.indexOf(":") + 2; i < line.size() - 1; i++)
				{
					tempCount += line[i];
				}
			}
			else
			{
				for (int i = line.indexOf(":") + 2; i < line.size(); i++)
				{
					tempCount += line[i];
				}
			}
			// Append ID and count at same time to avoid appending
			// the wrong ID, since there can be multiple in the reply
			vInvID.append(tempID);
			vInvCount.append(tempCount);
			tempCount.clear();
		}
	}
}

void ApiAccess::GetBankApiReply(QNetworkReply* reply, MaterialTracker* materialTracker)
{
	// Error getting reply from query
	if (reply->error())
	{
		error.NonModalErrorMessage(materialTracker, "Bank API Reply Error", reply->errorString());
		return;
	}

	// API answer
	QString answer = reply->readAll();

	// Parse the reply and add the information to the database
	// ID is used to match against the database to update the correct
	// material
	QString count;
	QString id;
	QTextStream text(&answer);
	while (!text.atEnd())
	{
		// Update the material tracker status label
		materialTracker->ChangeWaitingForApiReplyStatusLabel();

		QString line = text.readLine();

		// Parse the material id
		if (line.contains("\"id\":"))
		{
			id.clear();
			for (int i = line.indexOf(":") + 2; i < line.size() - 1; i++)
			{
				id += line[i];
			}
		}
		// Parse the material count
		if (line.contains("\"count\":"))
		{
			if (line.contains(","))
			{
				for (int i = line.indexOf(":") + 2; i < line.size() - 1; i++)
				{
					count += line[i];
				}
			}
			else
			{
				for (int i = line.indexOf(":") + 2; i < line.size(); i++)
				{
					count += line[i];
				}
			}

			vBankID.append(id);
			vBankCount.append(count);
			count.clear();
		}
	}
}

QString ApiAccess::QueryForMaterialIconURL(QString id)
{
	QUrl url("https://api.guildwars2.com/v2/items/" + id);
	QNetworkRequest request(url);
	QNetworkReply* urlReply = managerAccountMaterials->get(request);

	// Loop until we get the reply back
	// Ensures the application doesn't freeze while waiting
	QEventLoop loop;
	connect(urlReply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();

	// Process the reply and retrieve the url
	QString iconURL = GetMaterialIconUrlReply(urlReply);
	urlReply->deleteLater();

	return iconURL;
}

QString ApiAccess::GetMaterialIconUrlReply(QNetworkReply* reply)
{
	// Error getting reply from query
	if (reply->error())
	{
		qDebug() << reply->errorString();
		return reply->errorString();
	}

	// API answer
	QString answer = reply->readAll();

	// Parse the reply
	QString url;
	QTextStream text(&answer);
	while (!text.atEnd())
	{
		QString line = text.readLine();

		// Parse the icon url
		if (line.contains("icon"))
		{
			if (line.contains(","))
			{
				for (int i = line.indexOf(":") + 3; i < line.size() - 2; i++)
				{
					url += line[i];
				}
				break;
			}
			else
			{
				for (int i = line.indexOf(":") + 3; i < line.size() - 1; i++)
				{
					url += line[i];
				}
			}
		}
	}

	qDebug() << "URL: " << url;
	return url;
}

// Delays the program for a given amount of seconds
// Used to stall queries to the API network so that
// the server doesn't respond with an error
// Important for when building the databases
// since the network stopped responding after about 300
// queries without a minute cool down
void ApiAccess::delaySeconds(int secToWait)
{
    QTime dieTime = QTime::currentTime().addSecs( secToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

// Delays the program for a given amount of milliseconds
// Used to stall queries to the API network so that
// the server doesn't respond with an error
// Used to stall queries to the API network so that
// the server doesn't respond with an error
// Important for when building the databases
// since the network stopped responding after about 300
// queries without a minute cool down
void ApiAccess::delayMilliSeconds(int msecToWait)
{
    QTime dieTime = QTime::currentTime().addMSecs(msecToWait);
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}




