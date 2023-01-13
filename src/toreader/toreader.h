#ifndef TOREADER_H
#define TOREADER_H

#include <QWidget>

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

signals:
    void requestPage(int page);

private slots:
    void on_previousBtn_clicked();

    void on_nextBtn_clicked();

private:
    Ui::toreader *ui;
};

#endif // TOREADER_H
