#include "server.h"

Server::Server (int nPort, QWidget* pwgt) :
	QWidget(pwgt), NextBlockSize(0)
{
	this->nPort = nPort;
	ptcpServer = new QTcpServer(this);
	if (!ptcpServer->listen(QHostAddress::Any, nPort))
	{
		QMessageBox::critical(0, "Server Error", "Unable to start the server:" + ptcpServer->errorString());
		ptcpServer->close();
		return;
	}
	connect(ptcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
	ptxt = new QTextEdit;
	ptxt->setReadOnly(true);
	//Layout setup
	QVBoxLayout* pvbxLayout = new QVBoxLayout;
	pvbxLayout->addWidget(new QLabel("<H1>Server</H1>"));
	pvbxLayout->addWidget(ptxt);
	setLayout(pvbxLayout);
}
Server::Server(): NextBlockSize(0)
{

}
void Server::sendToClient(QTcpSocket* pSocket, QString& str)
{
	QByteArray arrBlock;
	QDataStream out(&arrBlock, QIODevice::WriteOnly);
	//out.setVersion(QDataStream::Qt_4_8);
	out << quint16(0) << QTime::currentTime() << str;
	out.device()->seek(0);
	out << quint16(arrBlock.size() - sizeof(quint16));
	pSocket->write(arrBlock);
}

bool Server::startServer()
{
	ptcpServer = new QTcpServer(this);
	if (!ptcpServer->listen(QHostAddress::Any, nPort))
	{
		ptcpServer->close();
		emit signal_startServerError(ptcpServer->errorString());
		return false;
	}
	connect(ptcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
	emit signal_display(QString("Server started successfully!!!"));
	localServer = new QTcpServer(this);
	localPort = 8888;
	if (!localServer->listen(QHostAddress::LocalHost, localPort))
	{
		localServer->close();
		emit signal_startServerError(localServer->errorString());
		return false;
	}
	return true;
}

QList<QString> Server::slot_getClients()
{
	QList<QString> cl;
	QMap<QString, QTcpSocket*>::iterator i = clients.begin();
	QList<QString>::iterator j = cl.begin();
	for (; i != clients.end(); ++i, ++j)
	{
		*j = i.key();
	}
	return cl;
}

void Server::slotNewConnection()
{
	QTcpSocket* pClientSocket = ptcpServer->nextPendingConnection();
	connect(pClientSocket, SIGNAL(disconnected()), pClientSocket, SLOT(deleteLater()));
	connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));
	QString str = "Server Response: Connected!!";
	QString* pstr = &str;
	sendToClient(pClientSocket, *pstr);
	QString buf = "New client connected: Новий клієнт" + pClientSocket->peerAddress().toString();
	emit signal_display(buf);
	clients.insert(pClientSocket->peerAddress().toString(), pClientSocket);
}
void Server::slotReadClient()
{
	QTcpSocket* pClientSocket = (QTcpSocket*)sender();
	QDataStream in(pClientSocket);
	//in.setVersion(QDataStream::Qt_4_8);
	for(;;)
	{
		if (!NextBlockSize)
		{
			if (pClientSocket->bytesAvailable() < sizeof(quint16)) break;
			in >> NextBlockSize;
		}
		if (pClientSocket->bytesAvailable() < NextBlockSize) break;
		QTime time;
		QString str;
		in >> time >> str;
		QString strMessage = time.toString() + " " + "Client has sent - " + str;
		//ptxt->append(strMessage);
		emit signal_display(strMessage);
		NextBlockSize = 0;
		str = "Server Response: Recived " + str + "/";
		QString *pstr = &str;
		sendToClient(pClientSocket, *pstr);
	}
}

void Server::slot_set_port(int argport)
{
	this->nPort = argport;
	this->startServer();

}

Server::~Server()
{
	delete ptcpServer;
	delete ptxt;
}
