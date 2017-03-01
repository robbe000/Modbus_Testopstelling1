#include "tabel.h"
#include "ui_tabel.h"

tabel::tabel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::tabel)
{
    ui->setupUi(this);
    this->setWindowTitle("Tabel");
    this->showMaximized();
    bool ok = fileInlezen();

    if(ok == false)
    {
        QMessageBox::warning(0, "Error", "Fout tijdens inlezen");
    }
}

tabel::~tabel()
{
    delete ui;
}

void tabel::setModbusDevice(QModbusClient *device)
{
    m_modbusDevice = device;
}

bool tabel::fileInlezen()
{
    QFile file("modbusAdressen.txt");
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
            items = buffer.split("; ");
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

void tabel::on_inlezen_clicked()
{
    int teller = 0;
    bool ok = true;
    bool globalOk = true;
    int adres;
    QModbusDataUnit unit;

    while(teller < this->ui->tabelWidget->rowCount())
    {
        globalOk = true;
        adres = this->ui->tabelWidget->item(teller, 2)->text().toInt(&ok);
        if(ok)
        {
            unit.setStartAddress(adres);
        }
        else
        {
            this->ui->tabelWidget->setItem(teller, 5, new QTableWidgetItem("Kan adres niet omvormen!")); //waarde
            globalOk = false;
        }

        if(this->ui->tabelWidget->item(teller, 3)->text() == "IR")
        {
            unit.setRegisterType(QModbusDataUnit::InputRegisters);
        }
        else
        {
            this->ui->tabelWidget->setItem(teller, 5, new QTableWidgetItem("Tabel niet herkent!")); //waarde
            globalOk = false;
        }

        if(this->ui->tabelWidget->item(teller, 1)->text() == "float")
        {
            unit.setValueCount(2);
        }
        else
        {
            this->ui->tabelWidget->setItem(teller, 5, new QTableWidgetItem("Type niet herkent!")); //waarde
            globalOk = false;
        }

        if(globalOk)
        {
            if(QModbusReply *antwoord = m_modbusDevice->sendReadRequest(unit, 1))
            {
                if(!antwoord->isFinished())
                {
                    this->ui->tabelWidget->setItem(teller, 5, new QTableWidgetItem("Wachten op response...")); //waarde
                    QList<int> lijst;
                    lijst.append(adres);
                    lijst.append(teller);
                    m_tabelLink.append(lijst);
                    connect(antwoord, &QModbusReply::finished, this, &tabel::lezen);
                }
            }
        }

        teller++;
    }
}

void tabel::lezen()
{
    QModbusReply *antwoord = qobject_cast<QModbusReply* >(sender());

    if(antwoord->error() == QModbusDevice::NoError)
    {
        bool gevonden = false;
        int teller = 0;

        qDebug() << "Geen error tijdens ontvangen van data.";

        const QModbusDataUnit unit = antwoord->result();

        while((teller < m_tabelLink.count()) & (gevonden == false))
        {
            QList<int> lijst = m_tabelLink.at(teller);
            if(lijst.at(0) == unit.startAddress())
            {
                const QModbusDataUnit unit = antwoord->result();
                QByteArray array;
                array.append(unit.value(1) & 0xff);
                array.append(unit.value(1) >> 8);
                array.append(unit.value(0) & 0xff);
                array.append(unit.value(0) >> 8);

                float getal = *(reinterpret_cast<const float*>(array.constData()));
                this->ui->tabelWidget->setItem(m_tabelLink.at(teller).value(1), 5, new QTableWidgetItem(QString::number(getal))); //waarde

                m_tabelLink.removeAt(teller);
                gevonden = true;
                qDebug() << "Gevonden";
            }
            teller++;
        }
    }

}













