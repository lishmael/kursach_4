#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QTextEdit>
#include <QFile>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
private:
		Ui::MainWindow *ui;
		QFile* fileHandler;
private slots:
		void on_buttonRun_clicked();
		void on_buttonStop_clicked();

		void on_buttonRestart_clicked();

	public slots:
	void slot_serverMessenger(QString disp);
	void slot_serverStarted(bool isStarted);
signals:
	void signal_startServer(int external, int internal);
	void signal_stopServer();

};

#endif // MAINWINDOW_H
