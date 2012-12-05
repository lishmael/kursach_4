#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QLineEdit *text = centralWidget()->findChild<QLineEdit*>(QString("InputPort"));
    int port = text->text().toInt();
    emit this->port_set(port);
}

void MainWindow::slot_serverMessenger(QString disp)
{
    QTextEdit *text = centralWidget()->findChild<QTextEdit*>(QString("server_messenger"));
    text->append(disp);
}

void MainWindow::on_clientList_activated(const QString &arg1)
{
    QList<QString> clients;
    emit signal_getClientList();
}
