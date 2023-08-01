#ifndef TOREADERFUNCTIONS_H
#define TOREADERFUNCTIONS_H

#include "toreader.h"
#include "functions.h"

#include <QString>
#include <QWidget>

// Functions for toreader, to make it tidy
void loadConfig();
void saveConfig();
void mainSetStyle(Ui::toreader *ui, toreader* thiss); // Main function calling others to set style, to make the code above better looking
void iconsSizeSet(Ui::toreader *ui, toreader* thiss);
void setFonts(Ui::toreader *ui, toreader* thiss);
void hideThings(Ui::toreader *ui, toreader* thiss);
void batteryWatchdog(toreader* thisRef);
void manageRecentBooksPages();

#endif // TOREADERFUNCTIONS_H
