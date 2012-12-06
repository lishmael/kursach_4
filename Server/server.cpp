#include "server.h"

/*Server::Server (int nPort, QWidget* pwgt) :
	QWidget(pwgt), NextBlockSize(0)
{
	this->externalPort = nPort;
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
}*/
Server::Server(): NextBlockSize(0)
{

	localServer = new QTcpServer(this);
	externalServer = new QTcpServer(this);
	connect(externalServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
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
	//External data interpriter
	if (!externalServer->listen(QHostAddress::LocalHost, externalPort))
	{
		externalServer->close();
		emit signal_startServerError(externalServer->errorString());
		return false;
	}

	emit signal_display(QString("External server started successfully at ") +
						externalServer->serverAddress().toString() + " : " +
						QString::number(externalServer->serverPort()));

	//Local data interpriter
	if (!localServer->listen(QHostAddress::LocalHost, localPort))
	{
		localServer->close();
		emit signal_startServerError(localServer->errorString());
		return false;
	}
	emit signal_display(QString("Internal server is ready at ") +
						localServer->serverAddress().toString() + " : " +
						QString::number(localServer->serverPort()));
	return true;
}

void Server::slotNewConnection()
{
	QTcpSocket* pClientSocket = externalServer->nextPendingConnection();
	QString sResponse = "Server Response: ";
	if (this->socketsConnected.indexOf(pClientSocket) == -1)
	{
		socketsConnected.push_back(pClientSocket);
		connect(pClientSocket, SIGNAL(disconnected()), pClientSocket, SLOT(deleteLater()));
		connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));
		sResponse += "Connected";
		QString buf = "New client connected: " +
					  pClientSocket->peerAddress().toString();
		emit signal_display(buf);
	}
	else
	{
		sResponse += "Error! Such client already exists!";
	}
	sendToClient(pClientSocket, sResponse);
}
/*void Server::slotReadClient()
{
	QTcpSocket* pClientSocket = (QTcpSocket*)sender();
	QDataStream in(pClientSocket);

	while (true)
	{
		if (!NextBlockSize)
		{
			if (pClientSocket->bytesAvailable() < sizeof(quint16))
				break;
			in >> NextBlockSize;
		}
		if (pClientSocket->bytesAvailable() < NextBlockSize)
			break;
		QTime time;
		QString str;
		in >> time >> str;
		QString strMessage = time.toString() + " " + "Client has sent - " + str;
		emit signal_display(strMessage);
		NextBlockSize = 0;
		str = "Server Response: Recived " + str + "/";
		QString *pstr = &str;
		sendToClient(pClientSocket, *pstr);
	}
}*/

void Server::slotReadClient()
{
	QTcpSocket* pClientSocket = (QTcpSocket*)sender();
	QDataStream in(pClientSocket);
	long totalDataRecived = 0;
	QTime time;
	QString buffer;
	while (true)
	{
		if (!NextBlockSize)
		{
			if (pClientSocket->bytesAvailable() < sizeof(quint16))
				break;
			in >> NextBlockSize;
			totalDataRecived += NextBlockSize;
		}
		if (pClientSocket->bytesAvailable() < NextBlockSize)
			break;
		QString sTmp;
		in >> time >> sTmp;
		buffer += sTmp;
		NextBlockSize = 0;
		sTmp = "Server Response: Recived " + sTmp;
		sendToClient(pClientSocket, sTmp);
	}

	QString strMessage = time.toString() + " " + "Client has sent " +
						 QString::number(totalDataRecived) +
						 " bytes. Data is: " + buffer;
	emit signal_display(strMessage);

}
void Server::slot_startServer(int externalPort, int localPort)
{
	this->externalPort = externalPort;
	this->localPort = localPort;
	emit signal_serverStaus(this->startServer());
}

void Server::slot_stopServer()
{
	for (size_t i = 0; i < socketsConnected.count(); ++i)
	{
		socketsConnected[i]->close();
		delete socketsConnected[i];
	}
	socketsConnected.clear();

	this->localServer->close();
	this->externalServer->close();

	emit this->signal_serverStaus(false);
	emit this->signal_display(QString("Server is down"));
}

Server::~Server()
{
	delete externalServer;
	delete localServer;
}
