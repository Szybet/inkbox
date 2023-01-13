#include "toreaderThread.h"
#include "functions.h"

toreaderThread::toreaderThread(QObject *parent)
    : QObject(parent)
{
}

void toreaderThread::receivedPage(int page) {
    log("Requested page " + QString::number(page), className);
    int forwardToCreate = 1;
    for(int i = page - forwardToCreate; i < page + forwardToCreate; i++) {
        if(i > 0) {
            getPage(i);
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
        pdfArgs << "-W" << "300"; // Width
        pdfArgs << "-H" << "430"; // height
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
