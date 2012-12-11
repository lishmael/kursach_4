#include "server.h"

Server::Server(): NextBlockSize(0)
{
	databaseConnectionEstablished = false;
	localServer = new QTcpServer(this);
	externalServer = new QTcpServer(this);
	connect(externalServer, SIGNAL(newConnection()), this, SLOT(slot_newConnection()));
	connect(localServer, SIGNAL(newConnection()), this, SLOT(slot_newDatabaseConnection()));
}

Server::~Server()
{
	delete externalServer;
	delete localServer;
}

bool Server::startServer()
{
	//External data interpriter
	if (!externalServer->listen(QHostAddress::LocalHost, externalPort))
	{
		externalServer->close();
		emit signal_startServerError("Local server starting error: " +
									 externalServer->errorString());
		return false;
	}

	emit signal_display(QString("External server started successfully at ") +
						externalServer->serverAddress().toString() + " : " +
						QString::number(externalServer->serverPort()));

	//Local data interpriter
	if (!localServer->listen(QHostAddress::LocalHost, localPort))
	{
		localServer->close();
		externalServer->close();
		emit signal_startServerError("Local server starting error: " +
									 localServer->errorString());
		return false;
	}
	emit signal_display(QString("Internal server is ready at ") +
						localServer->serverAddress().toString() + " : " +
						QString::number(localServer->serverPort()));
	return true;
}

void Server::slot_newConnection()
{
	QTcpSocket* pClientSocket = externalServer->nextPendingConnection();
	QString sResponse = "Server Response: ";
	if (this->socketsConnected.indexOf(pClientSocket) == -1)
	{
		socketsConnected.push_back(pClientSocket);
		connect(pClientSocket, SIGNAL(disconnected()), pClientSocket, SLOT(deleteLater()));
		connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slot_readClient()));
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

void Server::slot_readClient()
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

void Server::sendToClient(QTcpSocket* pSocket, QString& str)
{
	QByteArray arrBlock;
	QDataStream out(&arrBlock, QIODevice::WriteOnly);
	out << quint16(0) << QTime::currentTime() << str;
	out.device()->seek(0);
	out << quint16(arrBlock.size() - sizeof(quint16));
	pSocket->write(arrBlock);
	QTime time;
	QString strMessage = time.currentTime().toString() +
						 " Server has sent to the " +
						 pSocket->peerAddress().toString() + "\n" +
						 QString::number(arrBlock.size() - sizeof(quint16)) +
						 " bytes of data. Data is: " + str;
	emit signal_display(strMessage);
}

void Server::slot_databaseConnectionAboutToClose()
{
	databaseConnectionEstablished = false;
	delete databaseClientSocket;
}

void Server::slot_newDatabaseConnection()
{
	if (!databaseConnectionEstablished)
	{
		firstDBLine = true;
		databaseConnectionEstablished = true;
		databaseClientSocket = localServer->nextPendingConnection();
		connect(databaseClientSocket, SIGNAL(readyRead()),
				this, SLOT(slot_readDatabase()));
		connect(databaseClientSocket, SIGNAL(aboutToClose()),
				this, SLOT(slot_databaseConnectionAboutToClose()));
	}
}

void Server::slot_readDatabase()
{

	QHostAddress clientHost;
	QByteArray recivedData;
	while (databaseClientSocket->bytesAvailable() > 0)
	{
		char * buffer = new char[1024];
		//bzero(buffer, 1024);
		memset(buffer, 0, 1024);
		databaseClientSocket->readLine(buffer, 1024);
		if (firstDBLine)
		{
			if (!isIPV4Address( QHostAddress(QString(buffer)).toString()))
			{
				emit signal_display(QString("Incorrect incoming data from database server!"));
				return;
			}
			clientHost = QHostAddress(QString(buffer));
			firstDBLine = false;
		}
		else
		{
			recivedData.append(buffer, QString(buffer).count());
		}
	}
	emit signal_display(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz Recived: ") +
						QString::number(recivedData.count()) + QString(" bytes of data"));

	messageEncrypt(recivedData);
	QListIterator<QTcpSocket*> iter(socketsConnected);
	iter.toBack();
	QString sTmp = "";
	while (iter.hasPrevious())
	{
		QTcpSocket * tmp = iter.previous();
		if (tmp->peerAddress() == clientHost)
		{
			sTmp = QString(recivedData);
			sendToClient(tmp, sTmp);
			break;
		}
	}

}

bool Server::messageEncrypt(QByteArray &data)
{
	return false;
}

bool Server::messageDecrypt(QByteArray &data)
{
	return false;
}

bool Server::isIPV4Address(QString address)
{
	QStringList listAddress = address.split(".",QString::SkipEmptyParts);
	if (listAddress.count() == 4)
	{
		for (int i = 0; i < 4; ++i)
		{
			bool ok = false;
			int a = QString(listAddress[i]).toInt(&ok);
			if (ok)
			{
				if (a >= (i > 0 ? 0 : 1) && a <= 255)
					continue;
			}
			return false;
		}
	}
	return true;
}
