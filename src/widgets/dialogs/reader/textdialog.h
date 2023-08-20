#ifndef TEXTDIALOG_H
#define TEXTDIALOG_H

#include <QDialog>

#include "functions.h"

namespace Ui {
class textDialog;
}

class textDialog : public QDialog
{
    Q_OBJECT

public:
    explicit textDialog(QWidget *parent = nullptr);
    ~textDialog();

signals:
    void highlightText();

private slots:
    void on_highlightBtn_clicked();

private:
    Ui::textDialog *ui;
};

#endif // TEXTDIALOG_H
