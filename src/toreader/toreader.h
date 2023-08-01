#ifndef TOREADER_H
#define TOREADER_H

#include <QWidget>
#include "toreaderThread.h"
#include "functions.h"

namespace Ui {
class toreader;
}

class toreader : public QWidget
{
    Q_OBJECT

public:
    explicit toreader(QWidget *parent = nullptr);
    ~toreader();

    void setText(QString textProvided);
    global::toreader::toreaderConfig* conf;

    // Battery icons?
    QPixmap scaledChargingPixmap;
    QPixmap scaledHalfPixmap;
    QPixmap scaledFullPixmap;
    QPixmap scaledEmptyPixmap;

public slots:
    void receivedPage(QByteArray* data);

signals:
    void init(QString format);
    void requestPage(int page);

private slots:
    void on_previousBtn_clicked();
    void on_nextBtn_clicked();

private:
    Ui::toreader *ui;
    QThread *RequestThread;
    toreaderThread *toreaderThreadClass;
};

#endif // TOREADER_H
