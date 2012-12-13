#ifndef CLIENT_H
#define CLIENT_H


#include <QtNetwork/QtNetwork>
#include <QTcpServer>
#include <QTcpSocket>
#include <iostream>

class Client : public QObject
{
	Q_OBJECT
private:
	QTcpSocket* pTcpSocket;
	QTcpSocket* databaseClientSocket;
	QTcpServer* localServer;
	quint16 NextBlockSize;
	QFile* fileHandler;
	QString strHost;

	int externalPort, localPort;
	bool databaseConnectionEstablished;
	void loadConfig();
	bool messageEncrypt(QByteArray &data);
	bool messageDecrypt(QByteArray &data);

public:
	Client();
	~Client();
private slots:

//	void slotReadyRead();
	void slotError(QAbstractSocket::SocketError);
	void slotSendToServer(QByteArray message);
	void slotConnected();
	void slotDisplay(QString msg);
	void slot_readDatabase();
	void slot_newDatabaseConnection();
	void slot_databaseConnectionAboutToClose();
	void slot_readServer();
signals:
	void signal_Display(QString msg);
	void done();
};

#endif // CLIENT_H
