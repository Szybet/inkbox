#include "toreaderFunctions.h"

QString classNameTorFunc = "ToreaderFunctions";

void batteryWatchdog(toreader* thisRef) {
    // Battery watchdog
    if(global::reader::startBatteryWatchdog == true) {
        QTimer *t = new QTimer(thisRef);
        t->setInterval(2000);
        QObject::connect(t, &QTimer::timeout, [&]() {
            // Checking if battery level is low
            if(global::battery::showCriticalBatteryAlert != true) {
                ;
            }
            else {
                if(isBatteryCritical() == true) {
                    log("Warning! Battery is at a critical charge level!", classNameTorFunc);
                    //openCriticalBatteryAlertWindow(); // TODO
                }
            }

            if(global::battery::showLowBatteryDialog != true) {
                // Do nothing, since a dialog should already have been displayed and (probably) dismissed
                ;
            }
            else {
                if(isBatteryLow() == true) {
                    if(global::battery::batteryAlertLock == true) {
                        ;
                    }
                    else {
                        if(isUsbPluggedIn()) {
                            ;
                        }
                        else {
                            log("Warning! Battery is low!", classNameTorFunc);
                            //openLowBatteryDialog(); // TODO
                        }
                    }
                }
            }
        } );
        if(global::deviceID != "emu\n") {
            t->start();
        }
    }
}

QString openFile(toreader* thisRef, QString path) {
    QFile file(path);
    file.open(QIODevice::ReadOnly); // I believe it will work
    QString allText = file.readAll();
    file.close();
    log("Readed file text: " + allText, classNameTorFunc);

    // Here set all formattings
    return allText;
}

