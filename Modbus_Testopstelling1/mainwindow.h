#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModbusDataUnit>
#include <QModbusClient>
#include <QUrl>
#include <QModbusTcpClient>
#include <QMessageBox>
#include <writeregistermodel.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    QModbusDataUnit writeRequest() const;
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_schrijven_clicked();

private:
    Ui::MainWindow *ui;
    QModbusClient *m_modbusDevice = NULL;
    WriteRegisterModel *writeModel;
};

#endif // MAINWINDOW_H