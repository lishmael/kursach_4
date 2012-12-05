#include "mwserver.h"
#include "ui_mwserver.h"

MWServer::MWServer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MWServer)
{
    ui->setupUi(this);
}

MWServer::~MWServer()
{
    delete ui;
}

void MWServer::on_pushButton_pressed()
{
    quint16 nPort;
    QLabel* pNumPort = new QLabel;
}
