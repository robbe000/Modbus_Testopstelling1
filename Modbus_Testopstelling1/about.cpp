#include "about.h"
#include "ui_about.h"

about::about(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::about)
{
    ui->setupUi(this);
    this->setWindowTitle("About");
    this->ui->label->setWordWrap(true);
    this->ui->label_2->setText("Robbe Goovaerts");
}

about::~about()
{
    delete ui;
}
