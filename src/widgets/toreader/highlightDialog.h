#ifndef HIGHLIGHTDIALOG_H
#define HIGHLIGHTDIALOG_H

#include <QDialog>
#include "MouseClickEventFilter.h"

namespace Ui {
class highlightDialog;
}

class highlightDialog : public QDialog
{
    Q_OBJECT

public:
    explicit highlightDialog(QWidget *parent = nullptr);
    ~highlightDialog();
    MouseClickEventFilter* mouseClickFilter;
    void end();

public slots:
    void shutdownCheck(QPoint pos);

private slots:
    void on_addLeftButton_clicked();

    void on_addRightButton_clicked();

    void on_remLeftButton_clicked();

    void on_remRightButton_clicked();

    void on_highlightButton_clicked();

    void on_translateButton_clicked();

signals:
    void moveHighlight(int addLeft, int addRight);
    void highlightText();
    void translateText();

private:
    Ui::highlightDialog *ui;
};

#endif // HIGHLIGHTDIALOG_H
