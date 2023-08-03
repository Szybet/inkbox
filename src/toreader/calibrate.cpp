#include "calibrate.h"
#include "toreader.h"
#include "mupdfCaller.h"
#include "build/.ui/ui_toreader.h"

#include <QScrollBar>
#include <QDebug>

int calWidth = 100;
int calHeight = 50;
int page = 1;
int font = 18;
int stepValue = 10;
bool finishedReader = false;

void setMuPdf(toreader* thiss) {
    qDebug() << "Deinit mupdf start";
    //deInitMupdf();
    qDebug() << "Deinit mupdf stop, init start";
    //initMupdf(calWidth, calHeight, font, "preserve-images=yes", "html", global::toreader::filePath);
    tryReflowMuPdf(calWidth, calHeight, font);
    qDebug() << "init mupdf stop";
    QByteArray* data = getPageData(page);
    thiss->setText(QString::fromUtf8(*data)); // To avoid infinite loop because identical string
    finishedReader = true;
}

bool widthDone = false;
bool waitForRender = false;
bool widthPreciseCalibrate = false;
bool heightPreciseCalibrate = false;
int launchDelay = 70;
int maximumWidth= 99999;
int maximumHeight= 99999;
int times = 20;
bool passHeight = false;
bool passWidth = false;

void initCalibrate(int howManyPages, int fontSize, Ui::toreader *ui) {
    ui->text->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->text->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    times = howManyPages;
    font = fontSize;
    // Default values - reset
    widthDone = false;
    waitForRender = false;
    widthPreciseCalibrate = false;
    heightPreciseCalibrate = false;
    launchDelay = 70;
    maximumWidth= 99999;
    maximumHeight= 99999;
    passHeight = false;
    passWidth = false;

    calWidth = 100;
    calHeight = 50;
    page = 1;
    stepValue = 10;
    finishedReader = false;
}

void calibrate(toreader* thiss, Ui::toreader *ui) {
    qDebug() << "Launching calibrate" << calWidth << calHeight;
    //qDebug() << "Html text:" << ui->text->toHtml();
    if(waitForRender == false) {
        finishedReader = false;
        setMuPdf(thiss);
    }
    QTimer::singleShot(launchDelay, thiss, [thiss, ui] () {
        if(finishedReader == false) {
            waitForRender = true;
            QApplication::processEvents();
            QTimer::singleShot(launchDelay, thiss, &toreader::launchCalibrate);
            return;
        }
        if(widthDone == false) {
            qDebug() << "Calibrating width";
            if(ui->text->horizontalScrollBar()->isVisible() || passWidth == true) {
                if(maximumWidth > calWidth) {
                    maximumWidth = calWidth;
                }
                if(passWidth == false) {
                    calWidth = calWidth - stepValue / 2;
                }
                passWidth = false;
                if(widthPreciseCalibrate == false) {
                    widthPreciseCalibrate = true;
                    qDebug() << "Going into precise mode";
                } else {
                    qDebug() << "Finished calibrating width:" << calWidth;
                    widthDone = true;
                }
                QTimer::singleShot(launchDelay, thiss, &toreader::launchCalibrate);
            } else {
                if(widthPreciseCalibrate == false) {
                    calWidth = calWidth + stepValue;
                    if(calWidth > maximumWidth) {
                        calWidth = calWidth - stepValue;
                    }
                } else {
                    calWidth = calWidth + 1;
                    if(calWidth > maximumWidth) {
                        calWidth = calWidth - 1;
                        passWidth = true;
                    }
                }
                QTimer::singleShot(launchDelay, thiss, &toreader::launchCalibrate);
            }
        } else {
            if(ui->text->horizontalScrollBar()->isVisible()) {
                widthDone = false;
                widthPreciseCalibrate = false;
                calWidth = calWidth - stepValue * 3;
                QTimer::singleShot(launchDelay, thiss, &toreader::launchCalibrate);
                return;
            }
            qDebug() << "Calibrating height";
            if(ui->text->verticalScrollBar()->isVisible() || passHeight == true) {
                if(maximumHeight > calHeight) {
                    maximumHeight = calHeight;
                }
                if(passHeight == false) {
                    calHeight = calHeight - stepValue / 2;
                }
                passHeight = false;
                if(heightPreciseCalibrate == false) {
                    heightPreciseCalibrate = true;
                    qDebug() << "Going into precise mode";
                    QTimer::singleShot(launchDelay, thiss, &toreader::launchCalibrate);
                } else {
                    qDebug() << "Finished calibrating height:" << calHeight;
                    setMuPdf(thiss);
                    QTimer::singleShot(1500, thiss, [thiss, ui] () {
                        if(ui->text->verticalScrollBar()->isVisible() || ui->text->horizontalScrollBar()->isVisible()) {
                            qDebug() << "Finished calibrating but not really";
                            qDebug() << "Dump values:" << widthDone << calWidth << calHeight;
                            widthDone = !ui->text->horizontalScrollBar()->isVisible();
                            calWidth = (calWidth - stepValue) - stepValue / 3;
                            calHeight = (calHeight - stepValue) - stepValue / 3;
                            QTimer::singleShot(launchDelay, thiss, &toreader::launchCalibrate);
                        }
                        else {
                            if(times > 0) {
                                qDebug() << "Next iteration:" << times;
                                times = times - 1;
                                page = page + 1;
                                QTimer::singleShot(launchDelay, thiss, &toreader::launchCalibrate);
                                // Check if pages exist
                            } else {
                                qDebug() << "Finished!";
                                global::toreader::loadedConfig.width = calWidth;
                                global::toreader::loadedConfig.height = calHeight;
                                ui->text->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                                ui->text->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                                calculatePagesCount();
                            }
                        }
                    });
                    qDebug() << "Dump values:" << widthDone << calWidth << calHeight;
                }
            } else {
                if(heightPreciseCalibrate == false) {
                    calHeight = calHeight + stepValue;
                    if(calHeight > maximumHeight) {
                        calHeight = calHeight - stepValue;
                    }
                } else {
                    calHeight = calHeight + 1;
                    if(calHeight > maximumHeight) {
                        calHeight = calHeight - 1;
                        passHeight = true;
                    }
                }
                QTimer::singleShot(launchDelay, thiss, &toreader::launchCalibrate);
            }
        }
    });
}


