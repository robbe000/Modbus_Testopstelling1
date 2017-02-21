#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Modbus Lokale Test");
    this->ui->status->setText("Wachten op userinput");
    this->ui->poort->setText("192.168.7.145:502");

    //Coils klaar maken aan de hand van QCheckBoxes in een QVBoxLayout
    QVBoxLayout *coilsLayout = new QVBoxLayout;
    this->ui->coils->setLayout(coilsLayout);
    for(int i=0;i<10;i++)
    {
        m_coils.append(new QCheckBox);
        coilsLayout->addWidget(m_coils[i]);
    }

    //Discrete Inputs klaar maken aan de hand van QCheckBoxes in een QVBoxLayout
    QVBoxLayout *DILayout = new QVBoxLayout;
    this->ui->DI->setLayout(DILayout);
    for(int i=0;i<10;i++)
    {
        m_DI.append(new QCheckBox);
        DILayout->addWidget(m_DI[i]);
    }

    //Input Refistes klaar maken aan de hand van QLineEdits in een QVBoxLayout
    QVBoxLayout *IRLayout = new QVBoxLayout;
    this->ui->IR->setLayout(IRLayout);
    for(int i = 0;i<10;i++)
    {
        m_IR.append(new QLineEdit);
        IRLayout->addWidget(m_IR[i]);
    }

    //Holding Refistes klaar maken aan de hand van QLineEdits in een QVBoxLayout
    QVBoxLayout *HRLayout = new QVBoxLayout;
    this->ui->HR->setLayout(HRLayout);
    for(int i = 0;i<10;i++)
    {
        m_HR.append(new QLineEdit);
        HRLayout->addWidget(m_HR[i]);
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

//Verbinding maken
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

void MainWindow::on_pushButton_2_clicked()
{

    //coils inlezen
    if(QModbusReply *antwoordCoils = m_modbusDevice->sendReadRequest(QModbusDataUnit(QModbusDataUnit::Coils, 0, 10), 1))
    {
        if(!antwoordCoils->isFinished())
        {
            qDebug() << "Vraag coils goed verzonden.";
            connect(antwoordCoils, &QModbusReply::finished, this, &MainWindow::lezenCoils);
        }
    }

    //discrete inputs inlezen
    if(QModbusReply *antwoordDI = m_modbusDevice->sendReadRequest(QModbusDataUnit(QModbusDataUnit::DiscreteInputs, 0, 10), 1))
    {
        if(!antwoordDI->isFinished())
        {
            qDebug() << "Vraag discrete inputs goed verzonden.";
            connect(antwoordDI, &QModbusReply::finished, this, &MainWindow::lezenDI);
        }
    }

    //Input registers inlezen
    if(QModbusReply *antwoordIR = m_modbusDevice->sendReadRequest(QModbusDataUnit(QModbusDataUnit::InputRegisters, 0, 10), 1))
    {
        if(!antwoordIR->isFinished())
        {
            qDebug() << "Vraag input registers goed verzonden.";
            connect(antwoordIR, &QModbusReply::finished, this, &MainWindow::lezenIR);
        }
    }

    //Holding Registers inlezen
    if(QModbusReply *antwoordHR = m_modbusDevice->sendReadRequest(QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 0, 10), 1))
    {
        if(!antwoordHR->isFinished())
        {
            qDebug() << "Vraag input registers goed verzonden.";
            connect(antwoordHR, &QModbusReply::finished, this, &MainWindow::lezenHR);
        }
    }
}

void MainWindow::lezenCoils()
{
    int i;

    qDebug() << "Coils aan het lezen";

    QModbusReply *antwoord = qobject_cast<QModbusReply* >(sender());

    if(antwoord->error() == QModbusDevice::NoError)
    {
        qDebug() << "Geen error tijdens ontvangen van data.";

        const QModbusDataUnit unit = antwoord->result();

        for(i=0;i<10;i++)
        {
            m_coils[i]->setChecked(unit.value(i));
        }
    }

}

void MainWindow::lezenDI()
{
    int i;

    qDebug() << "DI aan het lezen";

    QModbusReply *antwoord = qobject_cast<QModbusReply* >(sender());

    if(antwoord->error() == QModbusDevice::NoError)
    {
        qDebug() << "Geen error tijdens ontvangen van data.";

        const QModbusDataUnit unit = antwoord->result();

        for(i=0;i<10;i++)
        {
            m_DI[i]->setChecked(unit.value(i));
        }
    }
}

void MainWindow::lezenIR()
{
    int i;

    qDebug() << "IR aan het lezen";

    QModbusReply *antwoord = qobject_cast<QModbusReply* >(sender());

    if(antwoord->error() == QModbusDevice::NoError)
    {
        qDebug() << "Geen error tijdens ontvangen van data.";

        const QModbusDataUnit unit = antwoord->result();

        for(i=0;i<10;i++)
        {
            m_IR[i]->setText(QString::number(unit.value(i)));
        }
    }
}

void MainWindow::lezenHR()
{
    int i;

    qDebug() << "HR aan het lezen";

    QModbusReply *antwoord = qobject_cast<QModbusReply* >(sender());

    if(antwoord->error() == QModbusDevice::NoError)
    {
        qDebug() << "Geen error tijdens ontvangen van data.";

        const QModbusDataUnit unit = antwoord->result();

        for(i=0;i<10;i++)
        {
            m_HR[i]->setText(QString::number(unit.value(i)));
        }
    }
}
