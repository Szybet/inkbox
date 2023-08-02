#include "toreaderThread.h"
#include "functions.h"
#include <unistd.h>
#include "toreader.h"
#include "mupdfCaller.h"

#include <QObject>
#include <QCoreApplication>
#include <QEventLoop>

toreaderThread::toreaderThread(QObject *parent)
    : QObject(parent)
{
}

void toreaderThread::initMuPdf(QString format) {

    // Test
    // global::toreader::loadedConfig.pagesCount = 80;
    if(format == "epub") {
        // For example, 280 and 280 is good for the nia??? and only with 9 font size
        // Ppi mess, ugh...
        //  1024 × 758
        global::toreader::loadedConfig.width = 280;
        global::toreader::loadedConfig.height = 280;

        initMupdf(global::toreader::loadedConfig.width, global::toreader::loadedConfig.height, 9, "preserve-images=yes", "html", global::toreader::filePath);
    }
    else {
        qDebug() << "Unknown format, oh no!";
    }
    isInit = true;
    global::toreader::pages.reserve(global::toreader::loadedConfig.pagesCount);
    getPage(global::toreader::loadedConfig.savedPage);

    // Testing
    /*
    for(int i = 11; i < 50; i++) {
        getPage(i);
    }
    */
}

void toreaderThread::getPage(int page) {
    qDebug() << "Getting page:" << page;
    QByteArray* data = getPageData(page);
    // writeFile("/tmp/page_" + QString::number(page) + ".html", QString::fromUtf8(*data));
    global::toreader::pages[page] = data;
    emit postPage(data);
    latestPage = page;
}
