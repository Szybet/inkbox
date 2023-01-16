#ifndef TOREADERTHREAD_H
#define TOREADERTHREAD_H

#include <QObject>
#include <QMutex>

class toreaderThread : public QObject
{
    Q_OBJECT

public:
    explicit toreaderThread(QObject *parent = nullptr);

    enum RequestType {
        Launch,
        Cache,
        Next,
        Back,
        GoTo,
    };
    Q_ENUM(RequestType);

    QString className = this->metaObject()->className();
    void getPage(int page);
    QVector<int> existingPages;
    void stopManage(QMutex& mutexOfThings, int& intToCheck, int valueToLookFor, bool& boolToSet, bool boolValue, QString logToGive);

public slots:
    void receivedPage(int page, RequestType request);

private:
};

#endif // TOREADERTHREAD_H
