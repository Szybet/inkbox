#include "translate.h"
#include "ui_translate.h"

#include "translateSettings.h"
#include "functions.h"

#include <iostream>

translate::translate(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::translate)
{
    ui->setupUi(this);
    loadTranslateConfig();
    this->setAttribute(Qt::WA_DeleteOnClose);

    ui->fromText->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    ui->toText->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
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

void translate::start(QString textFrom) {
    from = textFrom;
    refresh();
}

void translate::on_refreshButton_clicked() {
    refresh();
}

void translate::setStat(QString status) {
    ui->statusLabel->setText(status);
}

// Only for developers - set this to false always.
static bool translateDebug = true;
void translate::refresh() {
    if(checkWifiState() != global::wifi::wifiState::configured) {
        setStat("No internet connection");
        return;
    }
    if(translateDebug == true) {
        libreTranslateAPI = LibreTranslateAPI();
    }
    else {
        libreTranslateAPI = LibreTranslateAPI(global::translate::loadedConfig.url.toStdString(), global::translate::loadedConfig.apiKey.toStdString());
    }

    // nlohman, oh no
    libreTranslateAPI.languages();
    //qDebug() << "Languages:" << languages;

}
