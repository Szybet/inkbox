#ifndef DATASTREAMS_H
#define DATASTREAMS_H

#include <QMetaType>
#include <functions.h>

// Those need to be in the same file, and not in any namespace
Q_DECLARE_METATYPE(global::toreader::toreaderConfig); // And that's for QSettings
Q_DECLARE_METATYPE(global::translate::translateConfig); // And that's for QSettings
extern QDebug operator<<(QDebug dbg, const global::toreader::toreaderConfig& c);
extern QDebug operator<<(QDebug dbg, const global::translate::translateConfig& c);
// And here put all those Registers
// Very important: Call this after QApplication and before MainWindow
void declareMetaTypes();

#endif // DATASTREAMS_H
