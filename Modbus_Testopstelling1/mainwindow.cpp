#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->ui->status->setText("Wachten op userinput");
    this->ui->poort->setText("192.168.1.103:502");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    this->ui->status->setText("Nieuwe TCP client aanmaken...");

    //Nieuwe TCP client aanmaken
    m_modbusDevice = new QModbusTcpClient(this);

    this->ui->status->setText("Verbindng maken...");

    //Als er een error voorkomt -> weergeven in statusbalk
    connect(m_modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error){
        this->ui->status->setText(m_modbusDevice->errorString());
    });

    if(!m_modbusDevice)
    {
        this->ui->status->setText("Kon modbusDevice niet aanmaken!");
        return;
    }

    if(m_modbusDevice->state() != QModbusDevice::ConnectedState)
    {
        const QUrl url = QUrl::fromUserInput(this->ui->poort->text());
        m_modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter, url.port());
        m_modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter, url.host());
    }

    m_modbusDevice->setTimeout(1000);
    m_modbusDevice->setNumberOfRetries(3);

    if(!m_modbusDevice->connectDevice())
    {
        this->ui->status->setText("Connectie gefald: " + m_modbusDevice->errorString());
        return;
    }
    else
    {
        this->ui->status->setText("Connectie gelukt!");
    }
}

void MainWindow::on_schrijven_clicked()
{
    if(m_modbusDevice == NULL)
    {
        QMessageBox::warning(this, "Error", "modbusDevice bestaat niet!");
        return;
    }

    this->ui->status->setText("Aan het schrijven");

    QModbusDataUnit writeUnit = writeRequest();
    QModbusDataUnit::RegisterType table = writeUnit.registerType();

    if(table == QModbusDataUnit::Coils)
    {

    }

}

QModbusDataUnit MainWindow::writeRequest() const
{
    const auto table = QModbusDataUnit::Coils;

    int startAddress = 0;
    Q_ASSERT(startAddress >= 0 && startAddress < 10);

    // do not go beyond 10 entries
    int numberOfEntries = 1;
    return QModbusDataUnit(table, startAddress, numberOfEntries);
}
