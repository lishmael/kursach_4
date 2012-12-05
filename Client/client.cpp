#include "client.h"

Client::Client(const QString& strHost, int nPort, QWidget *pwgt)
	: QWidget(pwgt), NextBlockSize(0)
{
	pTcpSocket = new QTcpSocket(this);
	pTcpSocket->connectToHost(strHost, nPort);
	connect(pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
	connect(pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
	connect(pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
			this, SLOT(slotError(QAbstractSocket::SocketError)));
	ptxtInfo = new QTextEdit;
	ptxtInput = new QLineEdit;
	ptxtInfo->setReadOnly(true);
	QPushButton* pcmd = new QPushButton("&Send");
	connect(pcmd, SIGNAL(clicked()), SLOT(slotSendToServer()));
	connect(ptxtInput, SIGNAL(returnPressed()), this, SLOT(slotSendToServer()));
	//Layout setup
	QVBoxLayout* pvbx = new QVBoxLayout;
	pvbx->addWidget(new QLabel("<h1>Client</h1>"));
	pvbx->addWidget(ptxtInfo);
	pvbx->addWidget(ptxtInput);
	pvbx->addWidget(pcmd);
	setLayout(pvbx);
}
void Client::slotReadyRead()
{
	QDataStream in(pTcpSocket);
	for (;;)
	{
		if (!NextBlockSize)
		{
			if (pTcpSocket->bytesAvailable() < sizeof(quint16)) break;
			in >> NextBlockSize;
		}
		if (pTcpSocket->bytesAvailable() < NextBlockSize) break;
		QTime time;
		QString str;
		in >> time >> str;
		ptxtInfo->append(time.toString() + " " + str);
		NextBlockSize = 0;
	}
}
void Client::slotError(QAbstractSocket::SocketError)
{/*
	QString strError = "Error: " + (errno == QAbstractSocket::HostNotFoundError ? "The host was not found." :
									errno == QAbstractSocket::RemoteHostClosedError ? "The remote host is closed." :
									errno == QAbstractSocket::ConnectionRefusedError ? "The connection was refused." :
									QString(pTcpSocket->errorString()));
									*/
	//ptxtInfo->append(strError);
	ptxtInfo->append(QString("Connection error!"));
}
void Client::slotSendToServer()
{
	QByteArray arrBlock;
	QDataStream out(&arrBlock, QIODevice::WriteOnly);
	out << quint16(0) << QTime::currentTime() << ptxtInput->text();
	out.device()->seek(0);
	out << quint16(arrBlock.size() - sizeof(quint16));
	pTcpSocket->write(arrBlock);
	ptxtInput->setText("");
}
void Client::slotConnected()
{
	ptxtInfo->append("Recived the connected() signal");
}

Client::~Client()
{
	delete pTcpSocket;
	delete ptxtInfo;
	delete ptxtInput;
}
