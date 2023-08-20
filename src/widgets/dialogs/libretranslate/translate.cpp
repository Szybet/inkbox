#include "translate.h"
#include "ui_translate.h"

#include "translateSettings.h"

translate::translate(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::translate)
{
    ui->setupUi(this);
}

translate::~translate()
{
    delete ui;
}

void translate::on_settingsButton_clicked()
{
    translateSettings* settings = new translateSettings(this);
    settings->exec();
}

