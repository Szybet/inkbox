#ifndef CALIBRATE_H
#define CALIBRATE_H

#include "toreader.h"
#include "build/.ui/ui_toreader.h"

void calibrate(toreader* thiss, Ui::toreader *ui);
void initCalibrate(int howManyPages, int fontSize, Ui::toreader *ui);

#endif // CALIBRATE_H
