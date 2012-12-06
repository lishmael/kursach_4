#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	QLineEdit* to1 = centralWidget()->findChild<QLineEdit*>("inputExtPort");
	QLineEdit* to2 = centralWidget()->findChild<QLineEdit*>("inputIntPort");
	QWidget::setTabOrder(to1, to2);
    to1->setFocus();
//	fileHandler = new QFile("server.conf");
//	if (fileHandler->open(QFile::ReadOnly))
//	{
//		char buffer[64];
//		QString line;
//		while (fileHandler->readLine(buffer, sizeof(buffer)) != -1)
//		{
//			line = QString(buffer);
//			if (line[0] == '#')
//				continue;
//			else if (line[0] == '@')
//				if (line.indexOf("LocalPort") != -1)
//				{
//					line = line.rightRef(line.indexOf("LocalPort") +
//										 "LocalPort".count());
//				}
//				else if (line.indexOf("ExternalPort") != -1)
//				{
//					line = line.rightRef(line.indexOf("ExternalPort") +
//										 "ExternalPort".count());
//				}
//			else
//				{
//					continue;
//				}
//		}

//	}
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::slot_serverMessenger(QString disp)
{
	QTextEdit *text = centralWidget()->findChild<QTextEdit*>(QString("logArea"));
	text->append(disp);
	// TODO:
	// Write to logfile here like
	// pseudo: logfile->append(disp)
}

void MainWindow::slot_serverStarted(bool isStarted)
{
	if (isStarted)
	{
		centralWidget()->findChild<QPushButton*>(QString("buttonRun"))->setDisabled(true);
		centralWidget()->findChild<QLineEdit*>(QString("inputExtPort"))->setDisabled(true);
		centralWidget()->findChild<QLineEdit*>(QString("inputIntPort"))->setDisabled(true);
	}
	else
	{
		centralWidget()->findChild<QPushButton*>(QString("buttonRun"))->setDisabled(false);
		centralWidget()->findChild<QLineEdit*>(QString("inputExtPort"))->setDisabled(false);
		centralWidget()->findChild<QLineEdit*>(QString("inputIntPort"))->setDisabled(false);
	}
}


void MainWindow::on_buttonRun_clicked()
{
    QRegExp tocheck("[^0-9]");
    if (centralWidget()->findChild<QLineEdit*>(QString("inputExtPort"))->text().contains(tocheck) || centralWidget()->findChild<QLineEdit*>(QString("inputIntPort"))->text().contains(tocheck) ||
            centralWidget()->findChild<QLineEdit*>(QString("inputExtPort"))->text().length()!=4 || centralWidget()->findChild<QLineEdit*>(QString("inputIntPort"))->text().length()!=4)
    {
        QMessageBox::critical(0, "Invalid number of port(s)", "Port can only be a whole four significant number", QMessageBox::Ok, 0);
        centralWidget()->findChild<QLineEdit*>(QString("inputExtPort"))->clear();
        centralWidget()->findChild<QLineEdit*>(QString("inputIntPort"))->clear();
        return;
    }
	int extPort = centralWidget()->findChild<QLineEdit*>(QString("inputExtPort"))->text().toInt();
	int intPort = centralWidget()->findChild<QLineEdit*>(QString("inputIntPort"))->text().toInt();
	emit signal_startServer(extPort, intPort);

	fileHandler = new QFile("server.conf");
	fileHandler->open(QFile::WriteOnly | QFile::Truncate);
	fileHandler->write(QString("#Server configuration file\n\r").toStdString().c_str());
	fileHandler->write(QString("@LocalPort " +
							  QString::number(intPort) +
							  "\n\r").toStdString().c_str());
	fileHandler->write(QString("@ExternalPort " +
							   QString::number(extPort) +
							   "\n\r").toStdString().c_str());
	fileHandler->close();
	delete fileHandler;
	fileHandler = new QFile("client.conf");
	fileHandler->open(QFile::WriteOnly | QFile::Truncate);
	fileHandler->write(QString("#Client configuration file\n\r").toStdString().c_str());
	fileHandler->write(QString("@LocalPort <Write localport here>\n\r").toStdString().c_str());
	fileHandler->write(QString("@ServerIP <Write server ip here>\n\r").toStdString().c_str());
	fileHandler->write(QString("@ExternalPort " +
							   QString::number(extPort) +
							   "\`n\r").toStdString().c_str());

	fileHandler->close();
	delete fileHandler;
}

void MainWindow::on_buttonStop_clicked()
{
	emit signal_stopServer();
}

void MainWindow::on_buttonRestart_clicked()
{
	emit signal_stopServer();
	on_buttonRun_clicked();
}
