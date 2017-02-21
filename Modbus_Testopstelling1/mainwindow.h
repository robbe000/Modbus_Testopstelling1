#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "about.h"

#include <QMainWindow>
#include <QtWidgets>
#include <QModbusDataUnit>
#include <QModbusClient>
#include <QUrl>
#include <QModbusTcpClient>
#include <QMessageBox>
#include <QModbusDataUnit>
#include <QModbusReply>
#include <QDebug>

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

    void on_pushButton_2_clicked();
    void lezenCoils(void);
    void lezenDI(void);
    void lezenIR(void);
    void lezenHR(void);

    void on_pushButton_3_clicked();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    QModbusClient *m_modbusDevice = NULL;
    QList<QCheckBox*> m_coils;
    QList<QCheckBox*> m_DI;
    QList<QLineEdit*> m_IR;
    QList<QLineEdit*> m_HR;
};

#endif // MAINWINDOW_H
