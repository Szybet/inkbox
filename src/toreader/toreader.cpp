#include "toreader.h"
#include "ui_toreader.h"
#include "functions.h"
#include "toreaderFunctions.h"
#include "toreaderThread.h"
#include "libreader-rs.h"
#include "mupdfCaller.h"
#include "calibrate.h" // Ignore this error of redefinition lol

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
    initVarsForFun(ui, this);

    loadConfig();
    if(global::toreader::loadedConfig.pagesCount == -1) {
        // As everything is "on the go" we dont want a buffer overflow :D
        qDebug() << "Start to reserve fallback pages";
        global::toreader::pages.reserve(100);
        qDebug() << "Finished to reserve fallback pages";
    } else {
        global::toreader::pages.reserve(global::toreader::loadedConfig.pagesCount);
    }

    conf = &global::toreader::loadedConfig;
    conf->savedPage = 1;
    conf->skipEmptyPages = true;

    // Thread
    RequestThread = new QThread(this);
    toreaderThreadClass = new toreaderThread(); // No parent here, thats important!
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

    // QCoreApplication::processEvents();

    // Look
    mainSetStyle();

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
    batteryWatchdog();

    // Needed
    ui->gridLayout->setVerticalSpacing(0);

    // TODO
    ui->graphicsView->hide();


    ui->text->setWordWrapMode(QTextOption::NoWrap); // MuTool wraps words, and we trust it
}

toreader::~toreader()
{
    delete ui;
}

bool wasNextAsReason = true; // Default because for page 1 at fresh start
void toreader::setText(QString textProvided) {
    //log("Pure HTML code: \n" + htmlCode + "\n", className);

    // TODO: we dont request first page yet so... yea it won't work always
    bool containsImage = false;
    if(conf->imageAdjust == true || conf->skipEmptyPages == true) {
        containsImage = textProvided.contains("<img style=");
    }
    if(conf->skipEmptyPages == true) {
        bool skipPage = !containsImage;
        if(containsImage == false) {
            skipPage = !textProvided.contains("<span style=");
        }
        if(skipPage == true) {
            qDebug() << "Empty page detected, and we should skip it!";
            if(wasNextAsReason == true) {
                nextPage();
            }
            else {
                previousPage();
            }
            return void();
        }
    }

    // libreader-rs
    // TODO: WARNING: Possible memory leak, if qt doesn't manage qstring that well
    // more info:
    // https://github.com/Szybet/libreader-rs/blob/0e65478200ec02487eb081637b63ac18e73c242e/src/lib.rs#L145
    writeFile("/tmp/mupdf_pure.html", textProvided);
    textProvided = add_spaces(textProvided.toStdString().c_str());
    // qDebug() << "HTML code after adding spaces:" << textProvided;
    // writeFile("/tmp/mupdf_test_spaces.html", textProvided);

    textProvided = cut_off_head(textProvided.toStdString().c_str());
    //qDebug() << "HTML code after cutting off head:" << textProvided;
    // writeFile("/tmp/mupdf_test_cut_head.html", textProvided);

    // Mupdf changed something and its not longer needed in mupdf 1.23 ( latest, 2023-08-01 ), it was for sure in 1.20
    // Leaving it here, maybe it's needed for other formats
    // It's needed actually, but something changed

    writeFile("/tmp/mupdf_test_final.html", textProvided);
    // Yea i got problems with that too, needed logs
    qDebug() << "Setting text";

    ui->text->setHtml(textProvided);
    setTextStyle(&textProvided, containsImage);

    QCoreApplication::processEvents(QEventLoop::AllEvents);
    qDebug() << "Setted text";
}

void toreader::on_previousBtn_clicked()
{
    previousPage();
}

void toreader::on_nextBtn_clicked()
{
    nextPage();
}

void toreader::receivedPage(QByteArray* data) {
    setText(QString::fromUtf8(*data));
}

void toreader::launchCalibrate() {

    calibrate(this, ui);
}

void toreader::nextPage() {
    // We don't want to avoid going next when opening a fresh book
    if(conf->savedPage + 1 > global::toreader::loadedConfig.pagesCount && global::toreader::loadedConfig.pagesCount != -1) {
        showToast("You are on the last page");
    }
    else {
        conf->savedPage = conf->savedPage + 1;
        wasNextAsReason = true;
        emit requestPage(conf->savedPage);
        qDebug() << "Requesting page:" << conf->savedPage;
    }
}

void toreader::previousPage() {
    if(conf->savedPage == 0) {
        showToast("You are on the first page");
    }
    else {
        conf->savedPage = conf->savedPage - 1;
        wasNextAsReason = false;
        emit requestPage(conf->savedPage);
        qDebug() << "Requesting page:" << conf->savedPage;
    }
}
