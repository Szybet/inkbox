#include "translateSettings.h"
#include "ui_translateSettings.h"
#include "functions.h"
#include "datastreams.h" // Needed for qt declare metadata type
#include "generaldialog.h"

#include <QSettings>
#include <QScreen>

translateSettings::translateSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::translateSettings)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    loadTranslateConfig();

    int width = QGuiApplication::screens()[0]->size().width();
    int height = QGuiApplication::screens()[0]->size().height();
    this->setFixedSize(width, height - 400);
    this->move(0, 200);

    ui->urlButton->setIcon(QIcon("://resources/chevron-right.png"));
    ui->apiButton->setIcon(QIcon("://resources/chevron-right.png"));

    ui->label->setFont(QFont("u001", 10));

    ui->okButton->setProperty("type", "borderless");
    ui->urlButton->setProperty("type", "borderless");
    ui->apiButton->setProperty("type", "borderless");
    ui->infoButton->setProperty("type", "borderless");

    // Qt to render all borders ( and dont cut off text at reasonable place... ) needs a frame for example...
    QString styleSheet = "QPlainTextEdit {"
                         "    border: 1px solid black;"
                         "    border-radius: 10px;"
                         "}";

    ui->urlText->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    ui->urlText->setStyleSheet(styleSheet);


    ui->apiText->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    ui->apiText->setStyleSheet(styleSheet);

    ui->urlText->setPlainText(global::translate::loadedConfig.url);
    ui->apiText->setPlainText(global::translate::loadedConfig.apiKey);
}

translateSettings::~translateSettings()
{
    delete ui;
}

void translateSettings::on_urlButton_clicked()
{
    global::keyboard::keyboardDialog = true;
    global::keyboard::keyboardText = ui->urlText->toPlainText();
    generalDialog* generalDialogWindow = new generalDialog();
    generalDialogWindow->setAttribute(Qt::WA_DeleteOnClose);

    generalDialogWindow->exec();

    global::keyboard::keyboardDialog = false;
    if(global::keyboard::keyboardText.isEmpty() == false) {
        ui->urlText->setPlainText(global::keyboard::keyboardText);
        global::translate::loadedConfig.url = global::keyboard::keyboardText;
    }
    global::keyboard::keyboardText = "";
}

void translateSettings::on_apiButton_clicked()
{
    global::keyboard::keyboardDialog = true;
    global::keyboard::keyboardText = ui->apiText->toPlainText();
    generalDialog* generalDialogWindow = new generalDialog();
    generalDialogWindow->setAttribute(Qt::WA_DeleteOnClose);

    generalDialogWindow->exec();

    global::keyboard::keyboardDialog = false;
    if(global::keyboard::keyboardText.isEmpty() == false) {
        ui->apiText->setPlainText(global::keyboard::keyboardText);
        global::translate::loadedConfig.apiKey = global::keyboard::keyboardText;
    }
    global::keyboard::keyboardText = "";
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

void translateSettings::on_okButton_clicked()
{
    saveTranslateConfig();
    this->close();
}

void translateSettings::on_infoButton_clicked()
{
    // TODO: Add some explanations about api and servers
}

