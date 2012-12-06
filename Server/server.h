#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QtNetwork/QtNetwork>
#include <QTcpServer>
#include <QTextEdit>
#include <QTcpSocket>
#include <QMap>
#include <QList>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>

class Server : public QWidget
{
	Q_OBJECT
private:
	QTcpServer* externalServer;
	QTcpServer* localServer;
	quint16 NextBlockSize;
	int externalPort, localPort;
	QList<QTcpSocket*> socketsConnected;

	bool startServer();
	void sendToClient(QTcpSocket* pSocket, QString& str);
public:

	Server();
	//Server(int externalPort, QWidget *pwgt = 0);
	~Server();
public slots:
	virtual void slotNewConnection();
	void slotReadClient();
	void slot_startServer(int externalPort, int localPort);
	void slot_stopServer();
signals:
	void signal_startServerError(QString);
	void signal_display(QString);
	void signal_serverStaus(bool isOnline);
};

#endif // SERVER_H
