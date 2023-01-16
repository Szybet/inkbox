#include "toreaderThread.h"
#include "functions.h"
#include <unistd.h>
#include "toreader.h"

#include <QObject>
#include <QCoreApplication>
#include <QEventLoop>

toreaderThread::toreaderThread(QObject *parent)
    : QObject(parent)
{
}

void toreaderThread::receivedPage(int page, RequestType request) {
    log("Requested page " + QString::number(page), className);
    switch(request) {
        case Launch:
        {
            log("Requested Caching", className);
            getPage(page);
            mutex::boolSet(global::toreader::fileReady, global::toreader::FileReadyMutex, true);
            break;
        }
        case Cache:
        {
            log("Requested Caching", className);
            mutex::intPlus(global::toreader::stopCount, global::toreader::StopMutex);
            int forwardToCreate = 5;
            int minusCount = 2;
            for(int i = page + 1; i <= page + forwardToCreate; i++) {
                getPage(i);
                if(mutex::boolCheck(global::toreader::stop, global::toreader::StopMutex)) {
                    mutex::intMinus(global::toreader::stopCount, global::toreader::StopMutex);
                    stopManage(global::toreader::StopMutex, global::toreader::stopCount, 0, global::toreader::stop, false, "From Caching: Stop Count went to 0, Great");
                    log("Stopping caching", className);
                    return;
                };
                if(page - minusCount > 0) {
                    getPage(page - minusCount);
                    minusCount = minusCount + 1;
                    if(mutex::boolCheck(global::toreader::stop, global::toreader::StopMutex)) {
                        mutex::intMinus(global::toreader::stopCount, global::toreader::StopMutex);
                        log("Stopping caching", className);
                        stopManage(global::toreader::StopMutex, global::toreader::stopCount, 0, global::toreader::stop, false, "From Caching: Stop Count went to 0, Great");
                        return;
                    };
                }
            }
            // TODO: Here add pre opening files
            break;
        }
        case Next:
        {
            getPage(page);
            mutex::boolSet(global::toreader::fileReady, global::toreader::FileReadyMutex, true);
            break;
        }
        case Back:
        {
            getPage(page);
            mutex::boolSet(global::toreader::fileReady, global::toreader::FileReadyMutex, true);
            break;
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

        QProcess * proc = new QProcess();
        proc->start(pdfProg, pdfArgs);
        proc->waitForStarted();
        proc->deleteLater();

        mutex::intPlus(global::toreader::stopCount, global::toreader::StopMutex);
        while(true) {
            QCoreApplication::processEvents(QEventLoop::AllEvents);  // Not freeze the thread
            proc->waitForFinished(50); // Use as a delay
            if(proc->state() == QProcess::NotRunning) {
                break;
            }
            if(mutex::boolCheck(global::toreader::stop, global::toreader::StopMutex)) {
                log("Killing mutool", className);
                // TODO: Delete the file
                proc->kill();
                break;
            };
        }
        mutex::intMinus(global::toreader::stopCount, global::toreader::StopMutex);
        stopManage(global::toreader::StopMutex, global::toreader::stopCount, 0, global::toreader::stop, false, "From Mutool: Stop Count went to 0, Great");
        log("Mutool finished", className);

        if(proc->exitStatus() == QProcess::NormalExit) {
            existingPages.append(page);
        }
        else {
            // TODO: Delete the file
        }
    }
    else {
        log("Page " + QString::number(page) + " Already exists", className);
    }
}

void toreaderThread::stopManage(QMutex& mutexOfThings, int& intToCheck, int valueToLookFor, bool& boolToSet, bool boolValue, QString logToGive) {
    mutexOfThings.lock();
    if(intToCheck == valueToLookFor and boolToSet != boolValue) {
        boolToSet = boolValue;
        log(logToGive, className);
    }
    mutexOfThings.unlock();
}
