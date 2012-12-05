#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QTextEdit>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_pushButton_clicked();
    void on_clientList_activated(const QString &arg1);

public slots:
    void slot_serverMessenger(QString disp);

signals:
    void port_set(int);
    void signal_getClientList();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
