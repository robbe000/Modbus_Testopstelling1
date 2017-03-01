#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Modbus Test");
    this->ui->status->setText("Wachten op userinput");
    this->ui->poort->setText("10.98.4.25:502");
    this->ui->radio_coils->setChecked(1);

    //Menu tabs een naam geven
    this->ui->tabWidget->setTabText(0, "Voorbeeld PR");
    this->ui->tabWidget->setTabText(1, "Op adres");
    this->ui->tabWidget->setTabText(2, "Tabel");

    //Lees en schrijf knoppen uitschakelen
    this->ui->pushButton_2->setEnabled(0);
    this->ui->pushButton_3->setEnabled(0);
    this->ui->knop_adresInlezen->setEnabled(0);
    this->ui->tabelOpenen->setEnabled(0);

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
        m_DI[i]->setEnabled(0);
        DILayout->addWidget(m_DI[i]);
    }

    //Input Refistes klaar maken aan de hand van QLineEdits in een QVBoxLayout
    QVBoxLayout *IRLayout = new QVBoxLayout;
    this->ui->IR->setLayout(IRLayout);
    for(int i = 0;i<10;i++)
    {
        m_IR.append(new QLineEdit);
        m_IR[i]->setProperty("readOnly", 1);
        IRLayout->addWidget(m_IR[i]);
    }

    //Holding Refistes klaar maken aan de hand van QLineEdits in een QVBoxLayout
    QVBoxLayout *HRLayout = new QVBoxLayout;
    this->ui->HR->setLayout(HRLayout);
    for(int i = 0;i<10;i++)
    {
        m_HR.append(new QLineEdit);
        m_HR[i]->setProperty("readOnly", 1);
        HRLayout->addWidget(m_HR[i]);
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

QModbusClient *MainWindow::getModbusDevice()
{
    return m_modbusDevice;
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
        this->ui->pushButton_2->setEnabled(1);
        this->ui->pushButton_3->setEnabled(1);
        this->ui->knop_adresInlezen->setEnabled(1);
        this->ui->tabelOpenen->setEnabled(1);
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

void MainWindow::lezen()
{
    qDebug() << "HR aan het lezen";

    QModbusReply *antwoord = qobject_cast<QModbusReply* >(sender());

    if(antwoord->error() == QModbusDevice::NoError)
    {
        qDebug() << "Geen error tijdens ontvangen van data.";

        const QModbusDataUnit unit = antwoord->result();

        this->ui->uitkomst->setText(QString::number(unit.value(0)));
    }
}

void MainWindow::lezenFloat()
{
    qDebug() << "HR aan het lezen";

    QModbusReply *antwoord = qobject_cast<QModbusReply* >(sender());

    if(antwoord->error() == QModbusDevice::NoError)
    {
        qDebug() << "Geen error tijdens ontvangen van data.";

        const QModbusDataUnit unit = antwoord->result();
        QByteArray array;
        array.append(unit.value(1) & 0xff);
        array.append(unit.value(1) >> 8);
        array.append(unit.value(0) & 0xff);
        array.append(unit.value(0) >> 8);

        float getal = *(reinterpret_cast<const float*>(array.constData()));

        this->ui->uitkomst->setText(QString::number(getal));
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    QModbusDataUnit unitCoils = QModbusDataUnit(QModbusDataUnit::Coils, 0, 10);

    for(int i =0;i<10;i++)
    {
        unitCoils.setValue(i, m_coils[i]->checkState());
    }

    if(QModbusReply *antwoord = m_modbusDevice->sendWriteRequest(unitCoils, 1))
    {
        if(antwoord->error() != QModbusDevice::NoError)
        {
            this->ui->status->setText("Error tijdens schrijven" + antwoord->errorString());
        }
    }

}

void MainWindow::on_actionAbout_triggered()
{
    about *over = new about;
    over->show();
}

void MainWindow::on_knop_adresInlezen_clicked()
{
    QModbusDataUnit unit;

    //welk soort tabel
    if(this->ui->radio_coils->isChecked())
    {
        unit = QModbusDataUnit(QModbusDataUnit::Coils, this->ui->adres->text().toInt(), 1);
    }
    else if(this->ui->radio_DI->isChecked())
    {
        unit = QModbusDataUnit(QModbusDataUnit::DiscreteInputs, this->ui->adres->text().toInt(), 2);
    }
    else if(this->ui->radio_HR->isChecked())
    {
        unit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, this->ui->adres->text().toInt(), 2);
    }
    else if(this->ui->radio_IR->isChecked())
    {
        unit = QModbusDataUnit(QModbusDataUnit::InputRegisters, this->ui->adres->text().toInt(), 2);
    }
    else
    {
        //er is geen knop geselecteerd
        return;
    }

    if(this->ui->radio_coils->isChecked())
    {
        if(QModbusReply *antwoord = m_modbusDevice->sendReadRequest(unit, 1))
        {
            if(!antwoord->isFinished())
            {
                qDebug() << "Vraag goed verzonden.";
                connect(antwoord, &QModbusReply::finished, this, &MainWindow::lezen);
            }
        }
    }
    else
    {
        //Float waarde
        if(QModbusReply *antwoord = m_modbusDevice->sendReadRequest(unit, 1))
        {
            if(!antwoord->isFinished())
            {
                qDebug() << "Vraag goed verzonden.";
                connect(antwoord, &QModbusReply::finished, this, &MainWindow::lezenFloat);
            }
        }
    }
}

void MainWindow::on_tabelOpenen_clicked()
{
    tabel *tabelScherm = new tabel;
    tabelScherm->setModbusDevice(m_modbusDevice);
    tabelScherm->show();
}

void MainWindow::on_tabelOpenenNieuw_clicked()
{
    tabelNieuw *tabelScherm = new tabelNieuw;
    tabelScherm->setModbusDevice(m_modbusDevice);
    tabelScherm->show();
}
