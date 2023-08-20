#ifndef HIGHLIGHTDIALOG_H
#define HIGHLIGHTDIALOG_H

#include <QDialog>

namespace Ui {
class highlightDialog;
}

class highlightDialog : public QDialog
{
    Q_OBJECT

public:
    explicit highlightDialog(QWidget *parent = nullptr);
    ~highlightDialog();
    void shutdownCheck();

private slots:
    void on_addLeftButton_clicked();

    void on_addRightButton_clicked();

    void on_remLeftButton_clicked();

    void on_remRightButton_clicked();

signals:
    void moveHighlight(int addLeft, int addRight);

private:
    Ui::highlightDialog *ui;
    QTimer* touchTimer;
};

#endif // HIGHLIGHTDIALOG_H
