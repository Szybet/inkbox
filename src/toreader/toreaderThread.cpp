#include "toreaderThread.h"
#include "functions.h"
#include <unistd.h>

toreaderThread::toreaderThread(QObject *parent)
    : QObject(parent)
{
}

void toreaderThread::receivedPage(int page) {
    log("Requested page " + QString::number(page), className);
    if(secondCall == false) {
        getPage(page);
        global::toreader::FileReadyMutex.lock();
        global::toreader::fileReady = true;
        global::toreader::FileReadyMutex.unlock();
    }
    else {
        log("Second call", className);
        secondCall = true;
    }

    if(firstCall == true) {
        log("Calling second time because first page", className);
        firstCall = false;
        secondCall = true;
        // This timing is important, for the first call when opening reader
        QTimer::singleShot(500, this, [this, page] () {toreaderThread::receivedPage(page); });
        return void();
    }
    int forwardToCreate = 5;
    // Here prioritise next pages
    // TODO: This still freezes the gui somehow, maybe call those next pages after first ui->setText
    int minusCount = 2;
    for(int i = page + 1; i <= page + forwardToCreate; i++) {
        getPage(i);
        if(page - minusCount > 0) {
            getPage(page - minusCount);
            minusCount = minusCount + 1;
        }
    }
}

void toreaderThread::getPage(int page) {
    // does this contains function work
    if(existingPages.contains(page) == false) {
        log("Getting page " + QString::number(page), className);
        // TODO: here font, -X options etc
        QString pdfProg("/usr/local/bin/mutool");

        // This is more readable;
        QStringList pdfArgs;
        pdfArgs << "convert";
        pdfArgs << "-F" << "html"; // Format
        pdfArgs << "-X"; // Remove Epub layout
        pdfArgs << "-S" << "9"; // Font
        pdfArgs << "-W" << "315"; // Width
        pdfArgs << "-H" << "380"; // height
        pdfArgs << "-O";
        pdfArgs << "inhibit-spacespreserve-whitespace,dehyphenate";
        pdfArgs << "-o" << "/inkbox/book/split/" + QString::number(page); // Where to save it
        pdfArgs << global::toreader::filePath;
        pdfArgs << QString::number(page); // which page to get

        QProcess * pdfProc = new QProcess();
        pdfProc->start(pdfProg, pdfArgs);
        pdfProc->waitForFinished();
        pdfProc->deleteLater();

        log("Mutool finished", className);

        existingPages.append(page);
    }
    else {
        log("Page " + QString::number(page) + " Already exists", className);
    }
}
