#ifndef TOREADER_H
#define TOREADER_H

#include <QWidget>
#include "toreaderThread.h"

namespace Ui {
class toreader;
}

class toreader : public QWidget
{
    Q_OBJECT

public:
    explicit toreader(QWidget *parent = nullptr);
    ~toreader();
    QString className = this->metaObject()->className();
    bool isNightModeActive;
    // Battery icons?
    QPixmap scaledChargingPixmap;
    QPixmap scaledHalfPixmap;
    QPixmap scaledFullPixmap;
    QPixmap scaledEmptyPixmap;
    int currentPage = 1;
    void setText(QString path);
    void emitRequestPageFun(int page); // To call from singleshot

    // those functions can't be in toreader Functions because ui-> is private
    void mainSetStyle(); // Main function calling others to set style, to make the code above better looking
    void iconsSizeSet();
    void setFonts();
    void nightmode();
    void hideThings();

signals:
    void requestPage(int page);

private slots:
    void on_previousBtn_clicked();

    void on_nextBtn_clicked();

private:
    Ui::toreader *ui;
    QThread * RequestThread;
    toreaderThread* requestThreadFun;
};

#endif // TOREADER_H
