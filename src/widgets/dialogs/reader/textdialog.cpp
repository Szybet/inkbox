#include "textdialog.h"
#include "ui_textdialog.h"

textDialog::textDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::textDialog)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    ui->highlightBtn->setProperty("type", "borderless");
    ui->highlightBtn->setIcon(QIcon("://resources/apps.png"));
    this->adjustSize();
}

textDialog::~textDialog()
{
    delete ui;
}

void textDialog::on_highlightBtn_clicked()
{
    emit highlightText();
    // Make sure the signal is delivered
    QApplication::processEvents();
    this->close();
}
