#include "toreader.h"
#include "ui_toreader.h"
#include "functions.h"
#include "toreaderFunctions.h"
#include "toreaderThread.h"
#include "libreader-rs.h"
#include "mupdfCaller.h"
#include "calibrate.h" // Ignore this error of redefinition lol
#include "textdialog.h"
#include "highlightDialog.h"
#include "highlightslistdialog.h""

#include <QFontDialog>
#include <QWidget>
#include <QGraphicsScene>
#include <QFontDatabase>
#include <QScreen>
#include <QThread>
#include <QAbstractTextDocumentLayout>

toreader::toreader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::toreader)
{
    ui->setupUi(this);
    qDebug() << "Toreader launched";
    conf = &global::toreader::loadedConfig;
    initVarsForFun(ui, this, conf);

    loadConfig();
    if(global::toreader::loadedConfig.pagesCount == -1) {
        // As everything is "on the go" we dont want a buffer overflow :D
        qDebug() << "Start to reserve fallback pages";
        global::toreader::pages.reserve(100);
        qDebug() << "Finished to reserve fallback pages";
    } else {
        global::toreader::pages.reserve(global::toreader::loadedConfig.pagesCount);
    }

    // Thread
    RequestThread = new QThread(this);
    toreaderThreadClass = new toreaderThread(); // No parent here, thats important!
    toreaderThreadClass->moveToThread(RequestThread);
    RequestThread->start();
    // https://doc.qt.io/qt-6/qobject.html#connect
    // https://doc.qt.io/qt-6/qt.html#ConnectionType-enum
    connect(this, &toreader::init, toreaderThreadClass, &toreaderThread::initMuPdf, Qt::QueuedConnection);
    qDebug() << "Start init";
    emit init(conf->format);
    qDebug() << "End init";

    connect(this, &toreader::requestPage, toreaderThreadClass, &toreaderThread::getPage, Qt::QueuedConnection);
    connect(toreaderThreadClass, &toreaderThread::postPage, this, &toreader::receivedPage, Qt::QueuedConnection);
    // First page shown, saved one too
    emit requestPage(conf->savedPage);
    // QCoreApplication::processEvents();

    // Look
    mainSetStyle();

    // This needs to be here for whatever reason... ( here and only here )
    // Clock + battery level
    QTimer *t = new QTimer(this);
    t->setInterval(500);
    connect(t, &QTimer::timeout, [&]() {
       QString time = QTime::currentTime().toString("hh:mm");
       getBatteryLevel();
       ui->batteryLabel->setText(batteryLevel);
       ui->timeLabel->setText(time);
    } );
    t->start();
    batteryWatchdog();

    // Needed
    ui->gridLayout->setVerticalSpacing(0);

    // TODO
    ui->graphicsView->hide();

    ui->text->setWordWrapMode(QTextOption::NoWrap); // MuTool wraps words, and we trust it

    // Init some things
    highlightTimer = new QTimer(this);
    highlightTimer->setInterval(500);
    highlightTimer->setSingleShot(true);
    connect(highlightTimer, &QTimer::timeout, this, &toreader::highlightFunc);
}

toreader::~toreader()
{
    delete ui;
}

bool wasNextAsReason = true; // Default because for page 1 at fresh start
bool containsImage = false; // This can be used from other functions...
// Order is very important in this function
void toreader::setText(QString textProvided) {
    //log("Pure HTML code: \n" + htmlCode + "\n", className);

    // TODO: we dont request first page yet so... yea it won't work always???
    containsImage = textProvided.contains("<img style="); // Needed by highlighting
    if(conf->skipEmptyPages == true) {
        bool skipPage = !containsImage;
        if(containsImage == false) {
            skipPage = !textProvided.contains("<span style=");
        }
        if(skipPage == true) {
            qDebug() << "Empty page detected, and we should skip it!";
            if(wasNextAsReason == true) {
                nextPage();
            }
            else {
                previousPage();
            }
            return void();
        }
    }

    // libreader-rs
    // TODO: WARNING: Possible memory leak, if qt doesn't manage qstring that well
    // more info:
    // https://github.com/Szybet/libreader-rs/blob/0e65478200ec02487eb081637b63ac18e73c242e/src/lib.rs#L145
    writeFile("/tmp/mupdf_pure.html", textProvided);
    textProvided = add_spaces(textProvided.toStdString().c_str());
    // qDebug() << "HTML code after adding spaces:" << textProvided;
    // writeFile("/tmp/mupdf_test_spaces.html", textProvided);

    textProvided = cut_off_head(textProvided.toStdString().c_str());
    //qDebug() << "HTML code after cutting off head:" << textProvided;
    // writeFile("/tmp/mupdf_test_cut_head.html", textProvided);

    // Mupdf changed something and its not longer needed in mupdf 1.23 ( latest, 2023-08-01 ), it was for sure in 1.20
    // Leaving it here, maybe it's needed for other formats
    // It's needed actually, but something changed

    writeFile("/tmp/mupdf_test_final.html", textProvided);
    // Yea i got problems with that too, needed logs

    // Highlights
    // TODO: image + highlight support?
    if(containsImage == false) {
        //if(conf->loadHighlightsSlow == false) // And if they are enabled at all?
        QVector<QString> highlightList = getNormalHighlights();
        QString highlistsNative = convertToRustHighlights(highlightList);
        qDebug() << "The highlightList:" << highlightList;
        qDebug() << "Setting text";

        QString previousPage = getPageSlowSafe(conf->savedPage - 1);
        QString nextPage = getPageSlowSafe(conf->savedPage + 1);

        // No images for now
        if(previousPage.contains("<img style=")) {
            previousPage = "";
        }
        if(nextPage.contains("<img style=")) {
            nextPage = "";
        }

        const char* highlightedText = highlight_page_c(textProvided.toStdString().c_str(), previousPage.toStdString().c_str(), nextPage.toStdString().c_str(), highlistsNative.toStdString().c_str());
        textProvided = QString::fromStdString(highlightedText);
    }

    ui->text->setHtml(textProvided);
    setTextStyle(&textProvided, containsImage);
    writeFile("/tmp/mupdf_test_final2.html", textProvided);

    QCoreApplication::processEvents(QEventLoop::AllEvents);
    qDebug() << "Setted text";
}

void toreader::on_previousBtn_clicked()
{
    previousPage();
}

void toreader::on_nextBtn_clicked()
{
    nextPage();
}

void toreader::receivedPage(QByteArray* data) {
    setText(QString::fromUtf8(*data));
}

void toreader::launchCalibrate() {

    calibrate(this, ui);
}

void toreader::nextPage() {
    // We don't want to avoid going next when opening a fresh book
    if(conf->savedPage + 1 > global::toreader::loadedConfig.pagesCount && global::toreader::loadedConfig.pagesCount != -1) {
        showToast("You are on the last page");
    }
    else {
        conf->savedPage = conf->savedPage + 1;
        wasNextAsReason = true;
        emit requestPage(conf->savedPage);
        qDebug() << "Requesting page:" << conf->savedPage;
    }
}

void toreader::previousPage() {
    if(conf->savedPage == 0) {
        showToast("You are on the first page");
    }
    else {
        conf->savedPage = conf->savedPage - 1;
        wasNextAsReason = false;
        emit requestPage(conf->savedPage);
        qDebug() << "Requesting page:" << conf->savedPage;
    }
}

void toreader::on_optionsBtn_clicked()
{
    openMenu();
}

void toreader::setOnlyStyle() {
    // TODO???
    // I need in rust to remove whole span style because of font, sad
    emit requestPage(conf->savedPage);
    return;
    QString text = ui->text->toHtml();
    setTextStyle(&text, containsImage);
    if(text != ui->text->toHtml()) {
        ui->text->setHtml(text);
        setTextStyle(&text, containsImage);
    }
    QCoreApplication::processEvents(QEventLoop::AllEvents);
}

void toreader::on_fontBtn_clicked()
{
    QFontDialog* dialog = new QFontDialog(this);

    dialog->show();
    dialog->move(0, 0);

    // App wide option for this would be also `cool`
    QRect screenGeometry = QGuiApplication::screens()[0]->geometry();
    dialog->setFixedSize(screenGeometry.width(), screenGeometry.height());
    dialog->setFont(conf->font);
    QFont previousFont = conf->font;
    int result = dialog->exec();

    if(result == QDialog::Accepted) {
        conf->font = dialog->selectedFont();
        conf->fontSize = conf->font.pointSize();
        qDebug() << "New current font:" << conf->font;
        qDebug() << "New current font size:" << conf->fontSize;
        if(previousFont != conf->font) {
            setOnlyStyle();
        }
    }
}

void toreader::on_text_selectionChanged()
{
    qDebug() << "Selection changed";
    // I would want a isLocked function...
    if(highlightControl.tryLock()) {
        highlightControl.unlock();
        highlightTimer->stop();
        highlightTimer->start();
    }
}

void toreader::highlightFunc() {
    qDebug() << "highlightFunc called";
    if(highlightControl.tryLock() == false) {
        return;
    }
    qDebug() << "highlightFunc continue";

    repairSelection();

    // Highlight
    // TODO: deselect / remove highlight support

    textDialog* textDialogWindow = new textDialog(this);
    QObject::connect(textDialogWindow, &textDialog::destroyed, this, &toreader::unsetTextDialogLock);
    QObject::connect(textDialogWindow, &textDialog::highlightText, this, &toreader::highlightText);
    //QObject::connect(textDialogWindow, &textDialog::unhighlightText, this, &toreader::unhighlightText);
    QPoint pos = ui->text->cursorRect().bottomRight();
    // Shift it to not hide the text
    pos.setX(pos.x() + 30);
    pos.setY(pos.y() + 30);

    textDialogWindow->move(mapFromGlobal(pos));
    textDialogWindow->show();
}

QMutex highlightOngoing;
void toreader::unsetTextDialogLock() {
    // I miss is locked function
    if(highlightOngoing.tryLock()) {
        qDebug() << "unsetTextDialogLock called";
        highlightDelay();
    }
}

void toreader::highlightText() {
    highlightOngoing.lock();
    //Qt::TextEditorInteraction
    //Qt::NoTextInteraction
    ui->text->setTextInteractionFlags(Qt::NoTextInteraction); // wow it works as intended!

    //highlightBookText(selected_text, book_file, false);
    //setTextProperties(global::reader::textAlignment, global::reader::lineSpacing, global::reader::margins, global::reader::font, global::reader::fontSize);

    highlightDialog* dialog = new highlightDialog(this);
    connect(dialog, &highlightDialog::moveHighlight, this, &toreader::repairSelection);
    connect(dialog, &highlightDialog::destroyed, this, &toreader::highlightDelay);
    connect(dialog, &highlightDialog::highlightText, this, &toreader::highlightTextSlot);
    connect(dialog, &highlightDialog::translateText, this, &toreader::translateTextSlot);
    // welp
    int width = QGuiApplication::screens()[0]->size().width();
    int height = QGuiApplication::screens()[0]->size().height();
    int factor = 5;
    dialog->setFixedSize(QSize(width, height / factor));
    dialog->move(0, height - height / factor);
    dialog->show();
}

void toreader::highlightDelay() {
    QTimer::singleShot(300, this, [this] () {
        qDebug() << "Unlocked highlight";
        ui->text->setTextInteractionFlags(Qt::TextEditorInteraction);
        QTextCursor cursor = ui->text->textCursor();
        cursor.clearSelection();
        ui->text->setTextCursor(cursor);
        highlightControl.unlock();
        highlightOngoing.unlock();
    });
}

void toreader::repairSelection(int addLeft, int addRight) {
    qDebug() << "repairSelection called";
    QTextCursor cursor = ui->text->textCursor();
    QString text = ui->text->toPlainText();
    int length = ui->text->toPlainText().length();
    int positionStart = cursor.selectionStart();
    positionStart = positionStart + addLeft;
    int positionEnd = cursor.selectionEnd();
    positionEnd = positionEnd + addRight;
    bool startDone = false;
    bool endDone = false;
    qDebug() << "The text:" << text;
    while(true) {
        if(startDone == false) {
            if(positionStart >= 0 && positionStart < length) {
                qDebug() << "The char for start is:" << text[positionStart] << "At position" << positionStart;
                if(text[positionStart] == ' ' || text[positionStart] == '\n') {
                    startDone = true;
                }
                else {
                    if(addLeft > 0) {
                        positionStart = positionStart + 1;
                    }
                    else {
                        positionStart = positionStart - 1;
                    }
                }
            }
            else {
                startDone = true;
            }
        }

        if(endDone == false) {
            if(positionEnd >= 0 && positionEnd < length) {
                qDebug() << "The char for end is:" << text[positionEnd] << "At position" << positionEnd;
                if(text[positionEnd] == ' ' || text[positionEnd] == '\n') {
                    endDone = true;
                }
                else {
                    if(addRight >= 0) {
                        positionEnd = positionEnd + 1;
                    }
                    else {
                        positionEnd = positionEnd - 1;
                    }
                }
            }
            else {
                endDone = true;
            }
        }

        if(endDone == true && startDone == true) {
            break;
        }
    }

    //positionEnd = positionEnd - 1; // Idk?...
    positionStart = positionStart + 1;

    cursor.clearSelection();
    cursor.setPosition(positionStart);
    cursor.setPosition(positionEnd, QTextCursor::KeepAnchor);
    //cursor.movePosition(positionEnd, QTextCursor::MoveAnchor);

    // Doesn't work and is a mess
    /*
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::PreviousWord, QTextCursor::KeepAnchor);
    cursor.endEditBlock();
    */
    ui->text->setTextCursor(cursor);
}

void toreader::highlightTextSlot() {
    qDebug() << "Highlighting text was requested";
    QTextCursor cursor = ui->text->textCursor();
    QString selectedText = cursor.selectedText();
    highlightBookText(selectedText, global::toreader::filePath, false);
}

void toreader::translateTextSlot() {

}

void toreader::on_viewHighlightsBtn_clicked()
{
    qDebug() << "Launching highlights list dialog for book " + global::toreader::filePath;
    QJsonObject jsonObject = getHighlightsForBook(global::toreader::filePath);
    if(jsonObject.isEmpty() or jsonObject.length() <= 1) {
        global::toast::delay = 3000;
        showToast("No highlights for this book");
    }
    else {
        // This is awfull
        global::highlightsListDialog::bookPath = global::toreader::filePath;
        highlightsListDialog * highlightsListDialogWindow = new highlightsListDialog(this);
        // TODO: reflow page on exit
        connect(highlightsListDialogWindow, &highlightsListDialog::showToast, this, &toreader::showToastSlot);
        highlightsListDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
    }
}

void toreader::showToastSlot(QString text) {
    showToast(text);
}

QVector<QString> toreader::getNormalHighlights() {
    QVector<QString> vec;
    QJsonObject jsonObject = getHighlightsForBook(global::toreader::filePath);
    int keyCount = 1;
    foreach(const QString& key, jsonObject.keys()) {
        if(keyCount <= 1) {
            keyCount++;
            continue;
        }
        else {
            QString highlight = jsonObject.value(key).toString();
            vec.push_back(highlight);
        }
        keyCount++;
    }
    return vec;
}

QString toreader::convertToRustHighlights(QVector<QString> highlights) {
    QString finalStr;
    QString joinStr = "U+001F";
    for(int i = 0; i < highlights.length(); i++) {
        finalStr.push_back(highlights[i]);
        if(i != highlights.length() - 1) {
            finalStr.push_back(joinStr);
        }
    }
    qDebug() << "Final str highlights:" << finalStr;
    return finalStr;
}
