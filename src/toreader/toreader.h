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

    void nextPage();
    void previousPage();
    void setOnlyStyle();

    QString selectedText;
    QMutex highlightControl;

public slots:
    void receivedPage(QByteArray* data);
    void launchCalibrate(); // test?
    void highlightFunc();
    void unsetTextDialogLock();
    void highlightText();
    void highlightDelay();
    void repairSelection(int addLeft = 0, int addRight = 0); // Select only whole words

signals:
    void init(QString format);
    void requestPage(int page);

private slots:
    void on_previousBtn_clicked();
    void on_nextBtn_clicked();

    void on_optionsBtn_clicked();

    void on_fontBtn_clicked();

    void on_text_selectionChanged();

private:
    Ui::toreader *ui;
    QThread *RequestThread;
    toreaderThread *toreaderThreadClass;
    QTimer *highlightTimer;
};

#endif // TOREADER_H
