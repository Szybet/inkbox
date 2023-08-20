#ifndef TOREADERFUNCTIONS_H
#define TOREADERFUNCTIONS_H

#include "toreader.h"
#include "functions.h"
#include "datastreams.h" // Needed because of

#include <QString>
#include <QWidget>

// Needs to be called first!
void initVarsForFun(Ui::toreader *uiArg, toreader* thissArg, global::toreader::toreaderConfig* confArg);


// Functions for toreader, to make it tidy
void loadConfig();
void saveConfig();
void mainSetStyle(); // Main function calling others to set style, to make the code better looking
void iconsSizeSet();
void hideThings();
void batteryWatchdog();
void manageRecentBooksPages();
void showToast(QString messageToDisplay);
void setTextStyle(QString* textProvided, bool containsImage);
void setAlignment(); // 0 left, 1 center, 2 right, 3 justify
void openMenu();
QString getPageSlowSafe(int page);

#endif // TOREADERFUNCTIONS_H
