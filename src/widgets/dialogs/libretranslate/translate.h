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

private slots:
    void on_settingsButton_clicked();

private:
    Ui::translate *ui;
};

#endif // TRANSLATE_H
