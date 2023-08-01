#include "toreader.h"
#include "ui_toreader.h"
#include "functions.h"
#include "toreaderFunctions.h"
#include "toreaderThread.h"
#include "libreader-rs.h"
#include "mupdfCaller.h"

#include <QWidget>
#include <QGraphicsScene>
#include <QFontDatabase>
#include <QScreen>
#include <QThread>
#include <QAbstractTextDocumentLayout>

toreader::toreader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::toreader)
{
    ui->setupUi(this);
    qDebug() << "Toreader launched";

    loadConfig();

    conf = &global::toreader::loadedConfig;
    conf->savedPage = 25;

    // Thread
    RequestThread = new QThread(this);
    toreaderThreadClass = new toreaderThread(this);
    toreaderThreadClass->moveToThread(RequestThread);
    RequestThread->start();
    // https://doc.qt.io/qt-6/qobject.html#connect
    // https://doc.qt.io/qt-6/qt.html#ConnectionType-enum
    connect(this, &toreader::init, toreaderThreadClass, &toreaderThread::initMuPdf, Qt::QueuedConnection);
    qDebug() << "Start init";
    emit init("epub");
    qDebug() << "End init";
    connect(this, &toreader::requestPage, toreaderThreadClass, &toreaderThread::getPage, Qt::QueuedConnection);

    connect(toreaderThreadClass, &toreaderThread::postPage, this, &toreader::receivedPage, Qt::QueuedConnection);

    // TODO: QCoreApplication::processEvents();

    // Look
    mainSetStyle(ui, this);

    // This needs to be here for whatever reason... ( here and only here )
    // Clock + battery level
    QTimer *t = new QTimer(this);
    t->setInterval(500);
    connect(t, &QTimer::timeout, [&]() {
       QString time = QTime::currentTime().toString("hh:mm");
       getBatteryLevel();
       ui->batteryLabel->setText(batteryLevel);
       ui->timeLabel->setText(time);
    } );
    t->start();
    batteryWatchdog(this);

    // Needed
    ui->gridLayout->setVerticalSpacing(0);

    // TODO
    ui->graphicsView->hide();
}

toreader::~toreader()
{
    delete ui;
}

void toreader::setText(QString textProvided) {
    //log("Pure HTML code: \n" + htmlCode + "\n", className);

    // libreader-rs
    // TODO: WARNING: Possible memory leak, if qt doesn't manage qstring that well
    // more info:
    // https://github.com/Szybet/libreader-rs/blob/0e65478200ec02487eb081637b63ac18e73c242e/src/lib.rs#L145

    // writeFile("/tmp/mupdf_pure.html", textProvided);
    // textProvided = add_spaces(textProvided.toStdString().c_str());
    // qDebug() << "HTML code after adding spaces:" << textProvided;
    // writeFile("/tmp/mupdf_test_spaces.html", textProvided);

    textProvided = cut_off_head(textProvided.toStdString().c_str());
    //qDebug() << "HTML code after cutting off head:" << textProvided;
    // writeFile("/tmp/mupdf_test_cut_head.html", textProvided);

    // Mupdf changed something and its not longer needed in mupdf 1.23 ( latest, 2023-08-01 ), it was for sure in 1.20
    // Leaving it here, maybe it's needed for other formats

    // Yea i got problems with that too, needed logs
    qDebug() << "Setting text";
    ui->text->setHtml(textProvided);
    QCoreApplication::processEvents(QEventLoop::AllEvents);
    qDebug() << "Setted text";
}

void toreader::on_previousBtn_clicked()
{
    if(conf->savedPage != 0) {
        conf->savedPage = conf->savedPage - 1;
    }
    emit requestPage(conf->savedPage);
}

void toreader::on_nextBtn_clicked()
{
    // TODO: Last page detection
    conf->savedPage = conf->savedPage + 1;
    emit requestPage(conf->savedPage);
}

void toreader::receivedPage(QByteArray* data) {
    setText(QString::fromUtf8(*data));
}
