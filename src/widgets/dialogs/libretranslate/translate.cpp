#include "translate.h"
#include "ui_translate.h"

#include "translateSettings.h"
#include "functions.h"

#include <QScreen>

translate::translate(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::translate)
{
    ui->setupUi(this);
    loadTranslateConfig();
    this->setAttribute(Qt::WA_DeleteOnClose);

    ui->fromText->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    ui->toText->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);

    process = new QProcess(parent);

    timer = new QTimer(this);
    timer->setInterval(300);
    connect(timer, &QTimer::timeout, this, &translate::isTranslationReady);

    ui->fromBox->addItems(supportedLang);
    supportedLang.removeFirst(); // Remove auto
    ui->toBox->addItems(supportedLang);

    ui->refreshButton->setIcon(QIcon("://resources/refresh.png"));
    ui->refreshButton->setProperty("type", "borderless");
    ui->settingsButton->setIcon(QIcon("://resources/settings.png"));
    ui->settingsButton->setProperty("type", "borderless");

    // hehe
    int width = QGuiApplication::screens()[0]->size().width();
    int height = QGuiApplication::screens()[0]->size().height();
    this->setFixedSize(width, height);
    this->move(0, 0);
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
    ui->fromText->setText(from);
    refresh();
}

void translate::on_refreshButton_clicked() {
    refresh();
}

void translate::setStat(QString status) {
    ui->statusLabel->setText(status);
}

void translate::refresh() {
    ui->toText->clear();
    if(checkWifiState() != global::wifi::wifiState::configured) {
        setStat("No internet connection");
        return;
    }

    if(timer->isActive() == false && process->state() == QProcess::NotRunning) {
        translateText(from);
    }
    else {
        timer->stop();
        process->kill();
        translateText(from);
    }
}

// Debug url: https://translate.argosopentech.com/
void translate::translateText(QString text) {
    QString program = "./Libretranslate-rs-cli";
    if(QFile(program).exists() == false) {
        program = "/Libretranslate-rs-cli"; // Debugging
        if(QFile(program).exists() == false) {
            setStat("Libretranslate binary not found");
            return;
        }
    }
    if(process->state() == QProcess::Running || process->state() == QProcess::Starting) {
        process->kill();
    }

    QStringList arguments;
    arguments << text << ui->fromBox->currentText() << ui->toBox->currentText() << global::translate::loadedConfig.url << global::translate::loadedConfig.apiKey;
    qDebug() << "Arguments are:" << arguments;
    process->start(program, arguments);
    timer->start();
    setStat("Starting translating");
}

void translate::isTranslationReady() {
    //qDebug() << "isTranslationReady called";
    if(process->state() == QProcess::NotRunning) {
        timer->stop();
        QString text = process->readAllStandardOutput();
        if(text.isEmpty()) {
            text = process->readAllStandardError();
        }
        ui->toText->setText(text);
        setStat("Libretranslate binary finished");
    }
}

void translate::on_okButton_clicked()
{
    this->close();
}

