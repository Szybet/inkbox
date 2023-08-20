#include "highlightDialog.h"
#include "qdebug.h"
#include "qtimer.h"
#include "ui_highlightDialog.h"

highlightDialog::highlightDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::highlightDialog)
{
    ui->setupUi(this);

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

    touchTimer = new QTimer(this);
    touchTimer->setInterval(300);
    connect(touchTimer, &QTimer::timeout, this, &highlightDialog::shutdownCheck);
    touchTimer->start();
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

// Note: Obtaining the value of this property for a widget is effectively equivalent to checking whether QApplication::focusWidget() refers to the widget.
void highlightDialog::shutdownCheck() {
    QPoint globalCursorPos = this->keyboardGrabber(); // is he parent? idk... filter if this wont work
    qDebug() << "Has this dialog focus?" << globalCursorPos;
}
