#ifndef TOREADERTHREAD_H
#define TOREADERTHREAD_H

#include <QObject>

class toreaderThread : public QObject
{
    Q_OBJECT

public:
    explicit toreaderThread(QObject *parent = nullptr);
    QString className = this->metaObject()->className();
    void getPage(int page);
    QVector<int> existingPages;
    bool firstCall = true; // dsanmfjh
    bool secondCall = false;

public slots:
    void receivedPage(int page);

private:
};

#endif // TOREADERTHREAD_H
