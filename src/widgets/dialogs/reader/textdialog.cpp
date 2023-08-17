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
    if(global::reader::highlightAlreadyDone == true) {
        global::reader::highlightAlreadyDone = false;
        highlightAlreadyDone = true;
        ui->highlightBtn->setIcon(QIcon(":/resources/unhighlight.png"));
    }
    else {
        highlightAlreadyDone = false;
        ui->highlightBtn->setIcon(QIcon(":/resources/highlight.png"));
    }
    this->adjustSize();
}

textDialog::~textDialog()
{
    delete ui;
}

void textDialog::on_highlightBtn_clicked()
{
    if(highlightAlreadyDone == true) {
        emit unhighlightText();
    }
    else {
        emit highlightText();
    }
    textDialog::close();
}
