#include "translateSettings.h"
#include "ui_translateSettings.h"
#include "functions.h"
#include "datastreams.h" // Needed for qt declare metadata type

#include <QSettings>

translateSettings::translateSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::translateSettings)
{
    ui->setupUi(this);
    loadTranslateConfig();

    ui->urlButton->setIcon(QIcon("://resources/chevron-right.png"));
    ui->apiButton->setIcon(QIcon("://resources/chevron-right.png"));

    ui->label->setFont(QFont("u001", 10));

    ui->okButton->setProperty("type", "borderless");
    ui->urlButton->setProperty("type", "borderless");
    ui->apiButton->setProperty("type", "borderless");
    ui->infoButton->setProperty("type", "borderless");

    ui->urlText->setText(global::translate::loadedConfig.url);
    ui->apiText->setText(global::translate::loadedConfig.apiKey);
}

translateSettings::~translateSettings()
{
    delete ui;
}

void translateSettings::on_urlButton_clicked()
{

}

void translateSettings::on_apiButton_clicked()
{

}

QString defaultConfigPath = "/mnt/onboard/.adds/inkbox/.config/settings.ini";
void loadTranslateConfig() {
    QSettings *settingsGlobal = new QSettings(defaultConfigPath, QSettings::IniFormat);
    settingsGlobal->sync(); // Not needed i hope
    QVariant variant = settingsGlobal->value("translate");
    global::translate::loadedConfig = variant.value<global::translate::translateConfig>();
    settingsGlobal->deleteLater();
}

void saveTranslateConfig() {
    // Replace with default...
    QSettings *settingsGlobal = new QSettings(defaultConfigPath, QSettings::IniFormat);
    QVariant variant = QVariant::fromValue(global::translate::loadedConfig);
    settingsGlobal->setValue("translate", variant);
    settingsGlobal->sync();
    settingsGlobal->deleteLater();
}
