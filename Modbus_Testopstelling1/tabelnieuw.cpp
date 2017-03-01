#include "tabelnieuw.h"
#include "ui_tabelnieuw.h"

tabelNieuw::tabelNieuw(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::tabelNieuw)
{
    ui->setupUi(this);
    this->setWindowTitle("Nieuwe tabel weergave");
    this->showMaximized();

    bool ok = fileInlezen();

    if(!ok)
    {
        QMessageBox::warning(0, "Error", "Probleem tijdens inlezen van file!");
    }
}

tabelNieuw::~tabelNieuw()
{
    delete ui;
}

void tabelNieuw::setModbusDevice(QModbusClient *device)
{
    m_modbusDevice = device;
}

bool tabelNieuw::fileInlezen()
{
    QFile file("modbusAdressen.csv");
    QStringList tableHeader;
    QString buffer;
    QList<QString> items;

    //File openen
    if(!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    QTextStream in(&file);

    //Tabel klaarmaken
    tableHeader << "Naam" << "Type" << "Adres" << "Tabel" << "Comment" << "Waarde";
    this->ui->tabelWidget->clear();
    this->ui->tabelWidget->setColumnCount(6);
    this->ui->tabelWidget->setRowCount(0);
    this->ui->tabelWidget->setHorizontalHeaderLabels(tableHeader);
    this->ui->tabelWidget->setColumnWidth(0, 300);
    this->ui->tabelWidget->setColumnWidth(4, 700);
    this->ui->tabelWidget->setColumnWidth(5, 300);


    //File inlezen en tabel genereren
    while(!in.atEnd())
    {
        buffer = in.readLine();

        if(buffer != "")
        {
            items = buffer.split(";");
            this->ui->tabelWidget->insertRow(this->ui->tabelWidget->rowCount());

            if(items.count() == 5)
            {
                this->ui->tabelWidget->setItem(this->ui->tabelWidget->rowCount() - 1, 0, new QTableWidgetItem(items[0]));   //Naam
                this->ui->tabelWidget->setItem(this->ui->tabelWidget->rowCount() - 1, 1, new QTableWidgetItem(items[1]));   //Type
                this->ui->tabelWidget->setItem(this->ui->tabelWidget->rowCount() - 1, 2, new QTableWidgetItem(items[2]));   //Adres
                this->ui->tabelWidget->setItem(this->ui->tabelWidget->rowCount() - 1, 3, new QTableWidgetItem(items[3]));   //Tabel
                this->ui->tabelWidget->setItem(this->ui->tabelWidget->rowCount() - 1, 4, new QTableWidgetItem(items[4]));   //Commentaar
            }
            else
            {
                this->ui->tabelWidget->setItem(this->ui->tabelWidget->rowCount() - 1, 0, new QTableWidgetItem("Fout tijdens inlezen!"));   //Naam
                this->ui->tabelWidget->setItem(this->ui->tabelWidget->rowCount() - 1, 1, new QTableWidgetItem(""));   //Type
                this->ui->tabelWidget->setItem(this->ui->tabelWidget->rowCount() - 1, 2, new QTableWidgetItem(""));   //Adres
                this->ui->tabelWidget->setItem(this->ui->tabelWidget->rowCount() - 1, 3, new QTableWidgetItem(""));   //Tabel
                this->ui->tabelWidget->setItem(this->ui->tabelWidget->rowCount() - 1, 4, new QTableWidgetItem(""));   //Commentaar
            }
        }
    }

    //File sluiten
    file.close();

    return true;
}

//Deze functie start het inlezen van de waardes uit de PLC.
void tabelNieuw::startLezen()
{
    m_leescounter = 0;
    sendSingleRequest();
}

void tabelNieuw::sendSingleRequest()
{
    if(m_leescounter < this->ui->tabelWidget->rowCount())
    {
        //Gegevens uit tabel omvormen
        QModbusDataUnit unit;
        bool ok = true;
        bool globalOk = true;
        int adres = 0;
        int aantal = 0;

        //Adres
        adres = this->ui->tabelWidget->item(m_leescounter, 2)->text().toInt(&ok);
        if(!ok)
        {
            globalOk = false;
            this->ui->tabelWidget->setItem(m_leescounter, 5, new QTableWidgetItem("Kon adres niet omvormen."));
        }

        //Data type
        if(this->ui->tabelWidget->item(m_leescounter, 1)->text() == "float")
        {
            aantal = 2;
        }
        else
        {
            globalOk = false;
            this->ui->tabelWidget->setItem(m_leescounter, 5, new QTableWidgetItem("Data type niet herkent."));
        }

        //Tabel
        if(this->ui->tabelWidget->item(m_leescounter, 3)->text() == "IR")
        {
            unit.setRegisterType(QModbusDataUnit::InputRegisters);
        }
        else
        {
            globalOk = false;
            this->ui->tabelWidget->setItem(m_leescounter, 5, new QTableWidgetItem("Tabel type niet herkent."));
        }

        if(globalOk)
        {
            //Request klaarmaken en verzenden

            unit.setStartAddress(adres);
            unit.setValueCount(aantal);

            if(QModbusReply *antwoord = m_modbusDevice->sendReadRequest(unit, 1))
            {
                if(!antwoord->isFinished())
                {
                    connect(antwoord, &QModbusReply::finished, this, &tabelNieuw::lezen);
                }
            }
        }
        else
        {
            //Fout opgetreden

        }
    }
    else
    {
        //Tabel is volledig overlopen -> resetten en stoppen
        m_leescounter = 0;
    }
}


void tabelNieuw::on_inlezen_clicked()
{
    startLezen();
}

void tabelNieuw::lezen()
{
    QModbusReply *antwoord = qobject_cast<QModbusReply* >(sender());

    if(antwoord->error() == QModbusDevice::NoError)
    {
        if(this->ui->tabelWidget->item(m_leescounter, 1)->text() == "float")
        {
            const QModbusDataUnit unit = antwoord->result();
            QByteArray array;
            array.append(unit.value(1) & 0xff);
            array.append(unit.value(1) >> 8);
            array.append(unit.value(0) & 0xff);
            array.append(unit.value(0) >> 8);

            float getal = *(reinterpret_cast<const float*>(array.constData()));
            this->ui->tabelWidget->setItem(m_leescounter, 5, new QTableWidgetItem(QString::number(getal))); //waarde
        }
        else
        {
            this->ui->tabelWidget->setItem(m_leescounter, 5, new QTableWidgetItem("Data type niet herkent tijdens uitlezen."));
        }
    }
    else
    {
        this->ui->tabelWidget->setItem(m_leescounter, 5, new QTableWidgetItem("Error tijdens uitlezen!"));
    }

    m_leescounter++;
    sendSingleRequest();
}
