#ifndef MWSERVER_H
#define MWSERVER_H

#include <QMainWindow>
#include <server.h>

namespace Ui {
class MWServer;
}

class MWServer : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MWServer(QWidget *parent = 0);
    ~MWServer();
    
private slots:
    void on_pushButton_pressed();

private:
    Ui::MWServer *ui;
};

#endif // MWSERVER_H
