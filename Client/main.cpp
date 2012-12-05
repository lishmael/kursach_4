#include <QtGui/QApplication>
#include "client.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Client w("10.42.0.10", 2323);
	w.show();

	return a.exec();
}
