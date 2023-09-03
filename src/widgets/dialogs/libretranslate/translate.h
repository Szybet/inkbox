#ifndef TRANSLATE_H
#define TRANSLATE_H

#include <QDialog>
#include <QProcess>

namespace Ui {
class translate;
}

class translate : public QDialog
{
    Q_OBJECT

public:
    explicit translate(QWidget *parent = nullptr);
    QStringList supportedLang = { "auto", "en", "ar", "zh", "fr", "de", "it", "ja", "pt", "ru", "es", "pl"}; // This depends on support for https://github.com/Szybet/Libretranslate-rs-cli / I added pl
    ~translate();
    void start(QString textFrom);
    QString from;
    void refresh();
    void setStat(QString status);
    void translateText(QString text);
    void isTranslationReady();
    QString to;
    QProcess* process;

private slots:
    void on_settingsButton_clicked();

    void on_refreshButton_clicked();

    void on_okButton_clicked();

private:
    Ui::translate *ui;
    QTimer* timer;
};

#endif // TRANSLATE_H
