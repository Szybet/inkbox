#ifndef TOREADERTHREAD_H
#define TOREADERTHREAD_H

#include <QObject>

class toreaderThread : public QObject
{
    Q_OBJECT

public:
    explicit toreaderThread(QObject *parent = nullptr);
    QString className = this->metaObject()->className();
    void receivedPage(int page);
    void getPage(int page);
    QVector<int> existingPages;

private:
};

#endif // TOREADERTHREAD_H
