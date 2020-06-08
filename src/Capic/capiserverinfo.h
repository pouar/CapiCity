#ifndef CAPISERVERINFO_H
#define CAPISERVERINFO_H

#include <QString>
#include <QTcpSocket>

/**
 * @brief Contains Information about known servers
 */
class CapiServerInfo {
	public:
		CapiServerInfo(QString adress, int port);
		~CapiServerInfo();

		QString getAdress();
		int getPort();

		bool isOnline(); //Checks if the server is available.
		QString getVersion();

	private:
		QString adress; //The adress of the server
		int port; //The port of the server

		QTcpSocket* sock;
		QString version; //Determed at pinging server only
};

#endif // CAPISERVERINFO_H
