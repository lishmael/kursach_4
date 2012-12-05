#ifndef CLIENT_H
#define CLIENT_H

#include <QtGui/QWidget>
#include <QtNetwork/QtNetwork>
#include <QTcpServer>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QTcpSocket>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>

class Client : public QWidget
{
    Q_OBJECT
private:
    QTcpSocket* pTcpSocket;
    QTextEdit* ptxtInfo;
    QLineEdit* ptxtInput;
    quint16 NextBlockSize;
public:
    Client(const QString& strHost, int nPort, QWidget *pwgt = 0);
    ~Client();
private slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
    void slotSendToServer();
    void slotConnected();
};

#endif // CLIENT_H
