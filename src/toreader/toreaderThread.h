#ifndef TOREADERTHREAD_H
#define TOREADERTHREAD_H

#include <QObject>
#include <QMutex>

class toreaderThread : public QObject
{
    Q_OBJECT

public:
    explicit toreaderThread(QObject *parent = nullptr);

    /*
    enum RequestType {
        Restart,
        Cache,
        Next,
        Back,
        GoTo,
    };
    Q_ENUM(RequestType);
    */
    int latestPage = 0;


    //void startCache();
    //void stopCache();

public slots:    
    void initMuPdf(QString format);
    void getPage(int page);

signals:
    void postPage(QByteArray* data);

private:
};

#endif // TOREADERTHREAD_H
