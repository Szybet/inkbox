#include "highlightDialog.h"
#include "qdebug.h"
#include "qevent.h"
#include "qtimer.h"
#include "ui_highlightDialog.h"
#include "MouseClickEventFilter.h"

highlightDialog::highlightDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::highlightDialog)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    ui->addLeftButton->setIcon(QIcon("://resources/chevron-left.png"));
    ui->addRightButton->setIcon(QIcon("://resources/chevron-right.png"));
    ui->highlightButton->setIcon(QIcon("://resources/highlight.png"));
    ui->translateButton->setIcon(QIcon("://resources/translate.svg"));
    ui->remRightButton->setIcon(QIcon("://resources/chevron-left.png"));
    ui->remLeftButton->setIcon(QIcon("://resources/chevron-right.png"));


    ui->label->setFont(QFont("u001", 10));

    ui->addLeftButton->setProperty("type", "borderless");
    ui->addRightButton->setProperty("type", "borderless");
    ui->highlightButton->setProperty("type", "borderless");
    ui->translateButton->setProperty("type", "borderless");
    ui->remLeftButton->setProperty("type", "borderless");
    ui->remRightButton->setProperty("type", "borderless");

    mouseClickFilter = new MouseClickEventFilter(this->parentWidget());
    this->parentWidget()->installEventFilter(mouseClickFilter);
    connect(mouseClickFilter, &MouseClickEventFilter::Clicked, this, &highlightDialog::shutdownCheck);
}

highlightDialog::~highlightDialog()
{
    delete ui;
}

// This is not ideal, it could skip single character words
// If someone will complain, implement it going over 1 character, then if nothing changed over 2
void highlightDialog::on_addLeftButton_clicked()
{
    emit moveHighlight(-2, 0);
}

void highlightDialog::on_addRightButton_clicked()
{
    emit moveHighlight(0, 2);
}

void highlightDialog::on_remLeftButton_clicked()
{
    emit moveHighlight(2, 0);
}

void highlightDialog::on_remRightButton_clicked()
{
    emit moveHighlight(0, -2);
}

void highlightDialog::shutdownCheck(QPoint pos) {
    end();
}

void highlightDialog::on_highlightButton_clicked()
{
    emit highlightText();
    QApplication::processEvents();
    end();
}


void highlightDialog::on_translateButton_clicked()
{
    emit translateText();
    QApplication::processEvents();
    end();
}

void highlightDialog::end() {
    this->parentWidget()->removeEventFilter(mouseClickFilter);
    mouseClickFilter->deleteLater();
    this->close();
}
