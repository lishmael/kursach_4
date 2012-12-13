#include <QtCore>
#include <QApplication>

#include "client.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Client *client = new Client();
	QObject::connect(client, SIGNAL(done()), &a, SLOT(quit()));
	return a.exec();
}
