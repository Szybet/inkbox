#ifndef TRANSLATE_H
#define TRANSLATE_H

#include <QDialog>

namespace Ui {
class translate;
}

class translate : public QDialog
{
    Q_OBJECT

public:
    explicit translate(QWidget *parent = nullptr);
    ~translate();
    void start(QString textFrom);
    QString from;
    void refresh();
    void setStat(QString status);

private slots:
    void on_settingsButton_clicked();

    void on_refreshButton_clicked();

private:
    Ui::translate *ui;
};

#endif // TRANSLATE_H
