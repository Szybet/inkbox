#ifndef TOREADERFUNCTIONS_H
#define TOREADERFUNCTIONS_H

#include "toreader.h"
#include "functions.h"

#include <QString>
#include <QWidget>

// Functions for toreader, to make it tidy
void batteryWatchdog(toreader* thisRef);
QString openFile(toreader* thisRef, QString path);

#endif // TOREADERFUNCTIONS_H
