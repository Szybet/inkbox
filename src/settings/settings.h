#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>

#include <usbmsSplash.h>
#include "generaldialog.h"
#include "otamanager.h"
#include "toast.h"

using namespace std;

namespace Ui {
class settings;
}

class settings : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    int settings_page = 1;
    bool launch_sh = false;
    bool ui_enable_changed = false;
    bool ui_not_user_change = false;
    bool not_user_change = true;
    bool timezone_not_user_change = true;
    bool enableEncryptedStorageUserChange = false;

    explicit settings(QWidget *parent = nullptr);
    ~settings();

private slots:
    void on_demoCheckBox_toggled(bool);
    void on_aboutBtn_clicked();
    void on_okBtn_clicked();
    void on_clockCheckBox_toggled(bool checked);
    void on_quoteCheckBox_toggled(bool checked);
    void on_requestLeaseBtn_clicked();
    void on_usbmsBtn_clicked();
    void on_previousBtn_clicked();
    void on_nextBtn_clicked();
    void on_updateBtn_clicked();
    void on_darkModeCheckBox_toggled(bool checked);
    void on_uiScalingSlider_valueChanged(int value);
    void on_menuBarCheckBox_toggled(bool checked);
    void on_comboBox_currentIndexChanged(const QString &arg1);
    void on_resetBtn_clicked();
    void on_setPasscodeBtn_clicked();
    void on_enableLockscreenCheckBox_toggled(bool checked);
    void on_enableUiScalingCheckBox_toggled(bool checked);
    void on_showSystemInfoBtn_clicked();
    void on_readerScrollBarCheckBox_toggled(bool checked);
    void brightnessDown();
    void on_globalReadingSettingsCheckBox_toggled(bool checked);
    void on_checkOtaUpdateBtn_clicked();
    void openUpdateDialog();
    void launchOtaUpdater();
    void openUpdateDialogOTA(bool open);
    void showToastNative(QString messageToDisplay);
    void closeIndefiniteToastNative();
    void usbms_launch();
    void quit_restart();
    void on_enableEncryptedStorageCheckBox_toggled(bool checked);
    void disableStorageEncryption();
    void cancelDisableStorageEncryption();
    void on_repackBtn_clicked();
    void on_generateSystemReportBtn_clicked();
    void on_tzComboBox_currentTextChanged(const QString &arg1);
    void on_exportHighlightsBtn_clicked();
    void on_wordsNumberIncBtn_clicked();
    void on_wordsNumberDecBtn_clicked();
    void on_pageSizeHeightDecBtn_clicked();
    void on_pageSizeHeightIncBtn_clicked();
    void on_pageSizeWidthDecBtn_clicked();
    void on_pageSizeWidthIncBtn_clicked();
    void on_localLibraryShowFoldersCheckBox_toggled(bool checked);
    void on_usbmsDialogBox_clicked(bool checked);
    void on_autoCheckUpdatesBox_clicked(bool checked);

    void on_readerInfoButton_clicked();

signals:
    void showToast(QString messageToDisplay);
    void closeIndefiniteToast();

private:
    Ui::settings * ui;
    usbmsSplash * usbmsWindow;
    generalDialog * generalDialogWindow;
    otaManager * otaManagerWindow;
    toast * toastWindow;

    int pageSizeHeightSaved;
    int pageSizeWidthSaved;
    int wordsNumberSaved;
};

#endif // SETTINGS_H
