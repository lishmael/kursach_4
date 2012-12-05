#include <QtGui/QApplication>
#include "server.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server *srv = new Server();
    MainWindow *window = new MainWindow();

    QObject::connect(window, SIGNAL(port_set(int)), srv, SLOT(slot_set_port(int)) );
    QObject::connect(srv, SIGNAL(signal_startServerError(QString)),
                     window, SLOT(slot_serverMessenger(QString)));
    QObject::connect(srv, SIGNAL(signal_display(QString)), window, SLOT(slot_serverMessenger(QString)));
    QObject::connect(window, SIGNAL(signal_getClientList()), srv, SLOT(slot_getClients()));
    window->show();
    return a.exec();
}
