#ifndef TRANSLATESETTINGS_H
#define TRANSLATESETTINGS_H

#include <QDialog>

namespace Ui {
class translateSettings;
}

class translateSettings : public QDialog
{
    Q_OBJECT

public:
    explicit translateSettings(QWidget *parent = nullptr);
    ~translateSettings();

private slots:
    void on_urlButton_clicked();

    void on_apiButton_clicked();

    void on_okButton_clicked();

    void on_infoButton_clicked();

private:
    Ui::translateSettings *ui;
};

void loadTranslateConfig();
void saveTranslateConfig();

#endif // TRANSLATESETTINGS_H
