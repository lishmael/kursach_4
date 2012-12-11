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
	loadConfig();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::loadConfig()
{
	fileHandler = new QFile("server.conf");
	if (fileHandler->open(QFile::ReadOnly))
	{
		char buffer[64];
		QString line;
		intPort = -1;
		extPort = -1;
		while (fileHandler->readLine(buffer, sizeof(buffer)) != -1)
		{
			line = QString(buffer);
			QStringList inputRow = line.split(" ", QString::SkipEmptyParts);
			if (inputRow[0] == "@LocalPort")
			{
				intPort = inputRow[1].toInt();
			}
			else if (inputRow[0] == "@ExternalPort")
			{
				extPort = inputRow[1].toInt();
			}
			else
			{
				continue;
			}
		}
		QLineEdit * inputText = centralWidget()->findChild<QLineEdit*>("inputIntPort");
		inputText->setText(QString::number(intPort));
		inputText = centralWidget()->findChild<QLineEdit*>("inputExtPort");
		inputText->setText(QString::number(extPort));
		fileHandler->close();
	}
	delete fileHandler;
}

void MainWindow::writeConfig()
{
	QString endl = "\n\r";
	#ifndef Q_WS_WIN
	endl = "\n";
	#endif
	fileHandler = new QFile("server.conf");
	if (fileHandler->open(QFile::WriteOnly | QFile::Truncate))
	{
		fileHandler->write(QString("#Server configuration file" + endl).toStdString().c_str());
		fileHandler->write(QString("@LocalPort " + QString::number(intPort) + endl).toStdString().c_str());
		fileHandler->write(QString("@ExternalPort " + QString::number(extPort) + endl).toStdString().c_str());
		fileHandler->close();
	}
	else
	{
		QMessageBox::warning(this, "I/O error", "Cannot write server.config file!",
							 QMessageBox::Ok, 0);
	}
	delete fileHandler;
	fileHandler = new QFile("client.conf");
	if (fileHandler->open(QFile::WriteOnly | QFile::Truncate))
	{
		fileHandler->write(QString("#Client configuration file" + endl).toStdString().c_str());
		fileHandler->write(QString("@LocalPort <Write localport here>" + endl).toStdString().c_str());
		fileHandler->write(QString("@ServerIP <Write server ip here>" + endl).toStdString().c_str());
		fileHandler->write(QString("@ExternalPort " + QString::number(extPort) + endl).toStdString().c_str());
		fileHandler->close();
	}
	else
	{
		QMessageBox::warning(this, "I/O error", "Cannot write client.config file!",
							 QMessageBox::Ok, 0);
	}
	delete fileHandler;
}

void MainWindow::slot_serverMessenger(QString disp)
{
	QString endl = "\n\r";
	#ifndef Q_WS_WIN
	endl = "\n";
#endif
	QTextEdit *text = centralWidget()->findChild<QTextEdit*>(QString("logArea"));
	text->append(disp);
	fileHandler =  new QFile("server.log");
	if (fileHandler->open(QFile::WriteOnly | QFile::Append))
	{
		QString logString = QString(QDateTime::currentDateTime().toString("[dd.MM.yyyy hh:mm] ") +
									disp + endl);
		fileHandler->write(logString.toStdString().c_str());
		fileHandler->close();
	}
	else
	{
		QMessageBox::warning(this, "Logging error", "Cannot write to logfile!",
							 QMessageBox::Ok, 0);
	}
	delete fileHandler;
}

void MainWindow::slot_serverStarted(bool isStarted)
{
	isOnline = isStarted;
	if (isOnline)
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
	QRegExp inputChecker("[1-9]\\d{0,4}");
	QLineEdit * inputExtPort = centralWidget()->findChild<QLineEdit*>(QString("inputExtPort"));
	QLineEdit * inputIntPort = centralWidget()->findChild<QLineEdit*>(QString("inputIntPort"));
	if (!(inputChecker.exactMatch(inputExtPort->text()) &&
		  inputChecker.exactMatch(inputIntPort->text())))
	{
		QMessageBox::critical(0,
							  "Invalid port(s) number",
							  "Port can only be a number 1 - 99999",
							  QMessageBox::Ok,
							  0);
		inputIntPort->clear();
		inputExtPort->clear();
		return;
	}
	extPort = inputExtPort->text().toInt();
	intPort = inputIntPort->text().toInt();
	writeConfig();
	emit signal_startServer(extPort, intPort);
}

void MainWindow::on_buttonStop_clicked()
{
	if (isOnline)
		emit signal_stopServer();
}

void MainWindow::on_buttonRestart_clicked()
{
	on_buttonStop_clicked();
	on_buttonRun_clicked();
}
