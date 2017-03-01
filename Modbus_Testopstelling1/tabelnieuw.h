#ifndef TABELNIEUW_H
#define TABELNIEUW_H

#include <QWidget>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QModbusDataUnit>
#include <QModbusClient>
#include <QDebug>

namespace Ui {
class tabelNieuw;
}

class tabelNieuw : public QWidget
{
    Q_OBJECT

public:
    explicit tabelNieuw(QWidget *parent = 0);
    void setModbusDevice(QModbusClient *device);
    ~tabelNieuw();

private slots:
    void on_inlezen_clicked();
    void lezen(void);

private:
    Ui::tabelNieuw *ui;
    QModbusClient *m_modbusDevice = NULL;
    int m_leescounter = 0;
    bool fileInlezen(void);
    void startLezen(void);
    void sendSingleRequest(void);
};

#endif // TABELNIEUW_H
