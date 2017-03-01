#ifndef TABEL_H
#define TABEL_H

#include <QWidget>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QModbusDataUnit>
#include <QModbusClient>
#include <QDebug>
namespace Ui {
class tabel;
}

class tabel : public QWidget
{
    Q_OBJECT

public:
    explicit tabel(QWidget *parent = 0);
    ~tabel();
    void setModbusDevice(QModbusClient *device);

private slots:
    void on_inlezen_clicked();
    void lezen(void);

private:
    Ui::tabel *ui;
    bool fileInlezen(void);
    QModbusClient *m_modbusDevice = NULL;
    QList<QList<int>> m_tabelLink;
};

#endif // TABEL_H
