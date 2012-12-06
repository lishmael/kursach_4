#include <QtGui/QApplication>
#include "server.h"
#include "mainwindow.h"



int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Server *srv = new Server();
	MainWindow *window = new MainWindow();

	QObject::connect(window, SIGNAL(signal_startServer(int,int)),
					 srv, SLOT(slot_startServer(int,int)));
	QObject::connect(srv, SIGNAL(signal_startServerError(QString)),
					 window, SLOT(slot_serverMessenger(QString)));
	QObject::connect(srv, SIGNAL(signal_display(QString)),
					 window, SLOT(slot_serverMessenger(QString)));
	QObject::connect(srv, SIGNAL(signal_serverStaus(bool)),
					 window, SLOT(slot_serverStarted(bool)));
	QObject::connect(window, SIGNAL(signal_stopServer()),
					 srv, SLOT(slot_stopServer()));
	window->show();
	return a.exec();
}
