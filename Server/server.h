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
    QTcpServer* ptcpServer;
    QTcpServer* localServer;
    QTextEdit* ptxt;
    quint16 NextBlockSize;
    void sendToClient(QTcpSocket* pSocket, QString& str);
    QMap <QString, QTcpSocket*> clients;
    int nPort, localPort;
    bool startServer();
public:
    //explicit Server(QWidget *parent = 0);
    Server();
    Server(int nPort, QWidget *pwgt = 0);
    ~Server();
public slots:
    virtual void slotNewConnection();
    void slotReadClient();
    void slot_set_port(int);
    QList<QString> slot_getClients();

signals:
    void signal_startServerError(QString);
    void signal_display(QString);
};

#endif // SERVER_H
