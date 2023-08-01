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
    global::toreader::loadedConfig.pagesCount = 10;
    if(format == "epub") {
        // For example, 280 and 315 is good for the nia
        global::toreader::loadedConfig.width = 280;
        global::toreader::loadedConfig.height = 315;
        initMupdf(global::toreader::loadedConfig.width, global::toreader::loadedConfig.height, global::toreader::loadedConfig.fontSize, "preserve-images", "html", global::toreader::filePath);
    }
    else {
        qDebug() << "Unknown format, oh no!";
    }
    isInit = true;
    global::toreader::pages.reserve(global::toreader::loadedConfig.pagesCount);
    getPage(global::toreader::loadedConfig.savedPage);
}

void toreaderThread::getPage(int page) {
    QByteArray* data = getPageData(page);
    global::toreader::pages[page] = data;
    emit postPage(data);
    latestPage = page;
}
