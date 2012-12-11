#ifndef SERVER_H
#define SERVER_H

#include <QtNetwork/QtNetwork>
#include <QTcpServer>
#include <QTcpSocket>
#include <QWidget>
#include <QMessageBox>
#include <cryptopp/eccrypto.h>
#include <string>

class Server : public QWidget
{
	Q_OBJECT
private:
	QTcpServer* externalServer;
	QTcpServer* localServer;
	quint16 NextBlockSize;
	int externalPort;
	int localPort;
	QList<QTcpSocket*> socketsConnected;
	bool databaseConnectionEstablished;
	bool firstDBLine;
	QTcpSocket* databaseClientSocket;

	bool startServer();
	void sendToClient(QTcpSocket* pSocket, QString& str);
	bool messageEncrypt(QByteArray &data);
	bool isIPV4Address(QString address);
public:
	Server();
	~Server();
public slots:
	void slot_databaseConnectionAboutToClose();
	void slot_newDatabaseConnection();
	void slot_newConnection();
	void slot_readClient();
	void slot_readDatabase();
	void slot_startServer(int externalPort, int localPort);
	void slot_stopServer();
signals:
	void signal_startServerError(QString);
	void signal_display(QString);
	void signal_serverStaus(bool isOnline);
};

#endif // SERVER_H
