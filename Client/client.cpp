#include "client.h"

Client::Client() : NextBlockSize(0)
{
	connect(this, SIGNAL(signal_Display(QString)), this, SLOT(slotDisplay(QString)));
	loadConfig();
	localServer = new QTcpServer(this);
	if (!localServer->listen(QHostAddress::LocalHost, localPort))
	{
		localServer->close();
		emit signal_Display("Local server starting error: " + localServer->errorString());
	}
	emit signal_Display(QString("Internal server is ready at ") +
						localServer->serverAddress().toString() + " : " +
						QString::number(localServer->serverPort()));
	pTcpSocket = new QTcpSocket(this);
	pTcpSocket->connectToHost(strHost, externalPort);
	connect(pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
//	connect(pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
	connect(pTcpSocket, SIGNAL(readyRead()), SLOT(slot_readServer()));
	connect(pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
			this, SLOT(slotError(QAbstractSocket::SocketError)));
	connect(localServer, SIGNAL(newConnection()), this, SLOT(slot_newDatabaseConnection()));

}
void Client::loadConfig()
{
	localPort = -1;
	fileHandler = new QFile("client.conf");
	if (fileHandler->open(QFile::ReadOnly))
	{
		char buffer[64];
		QString line;
		localPort = -1;
		externalPort = -1;
		while (fileHandler->readLine(buffer, sizeof(buffer)) != -1)
		{
			line = QString(buffer);
			QStringList inputRow = line.split(" ", QString::SkipEmptyParts);
			if (inputRow[0] == "@LocalPort")
			{
				localPort = inputRow[1].toInt();
			}
			else if (inputRow[0] == "@ExternalPort")
			{
				externalPort = inputRow[1].toInt();
			}
			else if (inputRow[0] == "@ServerIP")
			{
				strHost = inputRow[1];
				strHost.truncate(strHost.length()-1);
			}
			else
			{
				continue;
			}
		}
		fileHandler->close();
	}
	else emit signal_Display(QString("Can not open 'client.conf' file."));
	delete fileHandler;
}
// Andrew's read from server
//void Client::slotReadyRead()
//{
//	QDataStream in(pTcpSocket);
//	for (;;)
//	{
//		if (!NextBlockSize)
//		{
//			if (pTcpSocket->bytesAvailable() < sizeof(quint16)) break;
//			in >> NextBlockSize;
//		}
//		if (pTcpSocket->bytesAvailable() < NextBlockSize) break;

//		QString str;
//		in >> str;
//		emit signal_Display(str);
//		NextBlockSize = 0;
//	}
//}
void Client::slot_readServer()
{
	QByteArray data;
	while (pTcpSocket->bytesAvailable() > 0)
	{
		data.append(pTcpSocket->readAll());
	}
	emit signal_Display("Read from server: " + QString(data));
}
void Client::slotError(QAbstractSocket::SocketError)
{
	/*
	QString strError = "Error: " + (errno == QAbstractSocket::HostNotFoundError ? "The host was not found." :
									errno == QAbstractSocket::RemoteHostClosedError ? "The remote host is closed." :
									errno == QAbstractSocket::ConnectionRefusedError ? "The connection was refused." :
									QString(pTcpSocket->errorString()));
	*/
	//ptxtInfo->append(strError);
	emit signal_Display("Connection error!");
	emit done();
}
void Client::slotSendToServer(QByteArray message)
{
	signal_Display(QString::number(message.count()) + " bytes was sent");
	pTcpSocket->write(message);
}
void Client::slotConnected()
{
	emit signal_Display("Connected to server");
}

void Client::slotDisplay(QString msg)
{
	QString logString = QDateTime::currentDateTime().toString("[dd.MM.yyyy hh:mm:ss.zzz] ");
	fileHandler =  new QFile("client.log");
	if (fileHandler->open(QFile::WriteOnly | QFile::Append))
	{
		logString += msg;
		fileHandler->write((logString + "\n").toStdString().c_str());
		fileHandler->close();
	}
	else
	{
		logString += "Logging error! Error writing 'client.log' file!";
	}
	delete fileHandler;
	std::cout << logString.toStdString() << std::endl;
}

Client::~Client()
{
	delete pTcpSocket;
	delete localServer;
}

void Client::slot_newDatabaseConnection()
{
	if (databaseConnectionEstablished)
	{
		delete databaseClientSocket;
		databaseConnectionEstablished = false;
	}

	databaseConnectionEstablished = true;
	databaseClientSocket = localServer->nextPendingConnection();
	connect(databaseClientSocket, SIGNAL(readyRead()),
			this, SLOT(slot_readDatabase()));
	connect(databaseClientSocket, SIGNAL(aboutToClose()),
			this, SLOT(slot_databaseConnectionAboutToClose()));

}

void Client::slot_readDatabase()
{


	QByteArray recivedData;
	while (databaseClientSocket->bytesAvailable() > 0)
	{
		char * buffer = new char[1024];
		//bzero(buffer, 1024);
		memset(buffer, 0, 1024);
		databaseClientSocket->readLine(buffer, 1024);
		recivedData.append(buffer, QString(buffer).count());
	}
	emit signal_Display("Recived " + QString::number(recivedData.count()) + " bytes of data");

	messageEncrypt(recivedData);
	slotSendToServer(recivedData);
	signal_Display("Data from internal: " + QString(recivedData));
}

void Client::slot_databaseConnectionAboutToClose()
{
	databaseConnectionEstablished = false;
	delete databaseClientSocket;
}



bool Client::messageEncrypt(QByteArray &data)
{
	return false;
}

bool Client::messageDecrypt(QByteArray &data)
{
	return false;
}
