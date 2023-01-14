#include "toreader.h"
#include "ui_toreader.h"
#include "functions.h"
#include "toreaderFunctions.h"
#include "toreaderThread.h"
#include "libreader-rs.h"

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
    log("Toreader launched", className);
    // In conclusion
    // this->layout()->setContentsMargins(0,0,0,0);
    // Causes a segault
    // this->setContentsMargins(0,0,0,0);
    // No effect
    // So i set it in the ui file, in "reader QWidget". Please don't change this, if it looks horrible on your device, contact me, then we need to figure something other out
    // Yes, i prefet the scroll bar to be on the edge...
    // ~Szybet
    // horizontalLayout is for line margins
    // To avoid uggly text cutting, also restored on option button click
    // ui->gridLayout->setVerticalSpacing(0);
    // this can't be launched here... so it will be launched at the end

    // Thread
    QThread * RequestThread = new QThread();
    toreaderThread* requestThreadFun = new toreaderThread(this);
    requestThreadFun->moveToThread(RequestThread);
    // https://doc.qt.io/qt-6/qobject.html#connect
    // https://doc.qt.io/qt-6/qt.html#ConnectionType-enum
    connect(this, &toreader::requestPage, requestThreadFun, &toreaderThread::receivedPage, Qt::DirectConnection);

    // TODO saved page
    currentPage = 10;

    // Important timing
    QTimer::singleShot(10, this, [this] () {toreader::emitRequestPageFun(currentPage); });
    log("Adjusting GUI look", className);

    // Look
    mainSetStyle();

    // Manage
    // .config/04-book/word-lookup-enabled
    // .config/04-book/highlighting-enabled

    /*
    if(!QFile(wordLookupEnablePath).exists()) {
        log("File " + wordLookupEnablePath + " doesn't exist, creating it", className);
        writeFile(wordLookupEnablePath, "true");
    }
    wordLookupEnabled = checkconfig(wordLookupEnablePath);
    ui->wordSearchBox->setChecked(wordLookupEnabled);
    log("Variable wordLookupEnabled is: " + QString::number(wordLookupEnabled), className);

    if(!QFile(highlightEnablePath).exists()) {
        log("File " + highlightEnablePath + " doesn't exist, creating it", className);
        writeFile(highlightEnablePath, "true");
    }
    highlightEnabled = checkconfig(highlightEnablePath);
    ui->highlightingBox->setChecked(highlightEnabled);
    log("Variable highlightEnabled is: " + QString::number(highlightEnabled), className);

    // Making text selectable
    if(highlightEnabled == true or wordLookupEnabled == true) {
        ui->text->setTextInteractionFlags(Qt::TextSelectableByMouse);
    }
    else {
        ui->text->setTextInteractionFlags(Qt::NoTextInteraction);
    }
    */

    // Encryption, I don't know if "/opt/inkbox_genuine" true means its native inkbox, so I don't know if it's this code
    /*
    if(checkconfig("/opt/inkbox_genuine") == true) {
        if(checkconfig("/external_root/run/encfs_mounted") == true) {
            QDir::setCurrent("/mnt/onboard/onboard/encfs-decrypted");
        }
        else {
            QDir::setCurrent("/mnt/onboard/onboard");
        }
        QFileDialog *dialog = new QFileDialog(this);
        // https://forum.qt.io/topic/29471/solve-how-to-show-qfiledialog-at-center-position-screen/4
        QDesktopWidget desk;
        QRect screenres = desk.screenGeometry(0); dialog->setGeometry(QRect(screenres.width()/4,screenres.height() /4,screenres.width()/2,screenres.height()/2));
        stylesheetFile.open(QFile::ReadOnly);
        dialog->setStyleSheet(stylesheetFile.readAll());
        stylesheetFile.close();
        book_file = dialog->getOpenFileName(dialog, tr("Open File"), QDir::currentPath());

        if(!book_file.isEmpty()) {
            setDefaultWorkDir();
        }
        else {
            // User clicked "Cancel" button
            // Restarting InkBox
            setDefaultWorkDir();
            QProcess process;
            process.startDetached("inkbox", QStringList());
            qApp->quit();
        }
        */

    // Use enum
    // TODO: What is this
    /*
    if(pdf_file_match(book_file) == true) {
        ui->line_4->setLineWidth(2);
    }
    */

    // Getting brightness level
    int brightness_value;
    if(global::isN705 == true or global::isN905C == true or global::isKT == true or global::isN873 == true) {
        brightness_value = get_brightness();
    }
    else if(global::isN613 == true) {
        setDefaultWorkDir();
        brightness_value = brightness_checkconfig(".config/03-brightness/config");
    }
    else {
        brightness_value = get_brightness();
    }
    ui->brightnessStatus->setValue(brightness_value);

    // Checking if there is a page refresh setting set
    string_checkconfig(".config/04-book/refresh");
    if(checkconfig_str_val == "") {
        // Writing the default, refresh every 3 pages
        string_writeconfig(".config/04-book/refresh", "3");
        string_checkconfig(".config/04-book/refresh");
    }
    else {
        // A config option was set, continuing after the Else statement...
        ;
    }
    /*
    pageRefreshSetting = checkconfig_str_val.toInt();
    // Checking if that config option was set to "Never refresh"...
    if(pageRefreshSetting == -1) {
        log("Setting page refresh to 'disabled'", className);
        neverRefresh = true;
    }
    else {
        // Safety measure
        log("Setting page refresh to each " + checkconfig_str_val + " pages", className);
        neverRefresh = false;
    }
    */

    // Clock setting to show seconds + battery level
    if(checkconfig(".config/02-clock/config") == true) {
        QTimer *t = new QTimer(this);
        t->setInterval(500);
        connect(t, &QTimer::timeout, [&]() {
           QString time = QTime::currentTime().toString("hh:mm:ss");
           get_battery_level();
           ui->batteryLabel->setText(batt_level);
           ui->timeLabel->setText(time);
        } );
        t->start();
    }
    else {
        QTimer *t = new QTimer(this);
        t->setInterval(500);
        connect(t, &QTimer::timeout, [&]() {
           QString time = QTime::currentTime().toString("hh:mm");
           get_battery_level();
           ui->batteryLabel->setText(batt_level);
           ui->timeLabel->setText(time);
        } );
        t->start();
    }

    // If needed, show scroll bar when rendering engine isn't doing its job properly
    if(checkconfig(".config/14-reader_scrollbar/config") == true) {
        ui->text->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
    else {
        ui->text->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    // Line spacing, margins and text alignment
    // Here also use enums ~Szybet
    /*
    QString lineSpacingString = readFile(".config/04-book/line_spacing");
    if(!lineSpacingString.isEmpty()) {
        global::reader::lineSpacing = lineSpacingString.toInt();
    }

    QString marginsString = readFile(".config/04-book/margins");
    if(!marginsString.isEmpty()) {
        global::reader::margins = marginsString.toInt();
    }

    QString alignment = readFile(".config/04-book/alignment");
    if(alignment == "Left") {
        global::reader::textAlignment = 0;
    }
    else if(alignment == "Center") {
        global::reader::textAlignment = 1;
    }
    else if(alignment == "Right") {
        global::reader::textAlignment = 2;
    }
    else if(alignment == "Justify") {
        global::reader::textAlignment = 3;
    }
    else {
        alignment = "Left";
        global::reader::textAlignment = 0;
    }
    log("Setting text alignment to '" + alignment + "'", className);

    // Don't ask me why it doesn't work without that QTimer
    QTimer::singleShot(0, this, SLOT(setInitialTextProperties()));
    */

    // Wheeee!
    // use enums bruh
    /*
    if(is_epub == true) {
        ui->graphicsView->hide();
        ui->graphicsView->deleteLater();
        ui->text->setText(epubPageContent);
        global::reader::currentViewportText = ui->text->toHtml();
    }
    else if(is_pdf == true) {
        ui->text->hide();
        ui->text->deleteLater();
        ui->line_20->hide();
        ui->line_20->deleteLater();
        ui->viewHighlightsBtn->hide();
        ui->viewHighlightsBtn->deleteLater();
        setupPng();
    }
    else if(is_image == true) {
        ui->text->hide();
        ui->text->deleteLater();
        ui->line_20->hide();
        ui->line_20->deleteLater();
        ui->viewHighlightsBtn->hide();
        ui->viewHighlightsBtn->deleteLater();
        setupPng();
    }
    */

    // Topbar info widget
    /*
    if(is_epub == true) {
        QString bookCreator = findEpubMetadata(book_file, "creator");
        QString bookTitle = findEpubMetadata(book_file, "title");
        bookCreator = bookCreator.trimmed();
        bookTitle = bookTitle.trimmed();

        QString infoLabelContent = bookCreator;
        infoLabelContent.append(" ― ");
        infoLabelContent.append(bookTitle);
        int infoLabelLength = infoLabelContent.length();
        int infoLabelDefinedLength;
        if(global::deviceID == "n705\n") {
            infoLabelDefinedLength = 35;
        }
        else if(global::deviceID == "n905\n" or global::deviceID == "n613\n" or global::deviceID == "n236\n" or global::deviceID == "n437\n" or global::deviceID == "n306\n" or global::deviceID == "kt\n" or global::deviceID == "emu\n") {
            infoLabelDefinedLength = 50;
        }
        else {
            infoLabelDefinedLength = 50;
        }

        if(infoLabelLength <= infoLabelDefinedLength) {
            ui->bookInfoLabel->setWordWrap(false);
        }
        else {
            ui->bookInfoLabel->setWordWrap(true);
        }
        ui->bookInfoLabel->setText(infoLabelContent);
    }
    else {
        QString bookReadRelativePath;
        if(wakeFromSleep == true) {
            string_checkconfig_ro("/tmp/inkboxBookPath");
            bookReadRelativePath = checkconfig_str_val.split("/").last();
        }
        else {
            bookReadRelativePath = book_file.split("/").last();
        }

        int infoLabelDefinedLength;
        if(global::deviceID == "n705\n") {
            infoLabelDefinedLength = 35;
        }
        else if(global::deviceID == "n905\n" or global::deviceID == "n613\n" or global::deviceID == "n236\n" or global::deviceID == "n437\n" or global::deviceID == "n306\n" or global::deviceID == "kt\n" or global::deviceID == "emu\n") {
            infoLabelDefinedLength = 50;
        }
        else {
            infoLabelDefinedLength = 50;
        }

        if(bookReadRelativePath <= infoLabelDefinedLength) {
            ui->bookInfoLabel->setWordWrap(false);
        }
        else {
            ui->bookInfoLabel->setWordWrap(true);
        }
        ui->bookInfoLabel->setText(bookReadRelativePath);
    }
    */

    // Way to tell shell scripts that we're in the Reader framework
    // string_writeconfig("/tmp/inkboxReading", "true");
    // whaaat ~Szybet

    // Maintain a 'Recent books' list
    QJsonObject recentBooksObject;
    if(QFile::exists(global::localLibrary::recentBooksDatabasePath)) {
        log("Reading recent books database", className);
        QFile recentBooksDatabase(global::localLibrary::recentBooksDatabasePath);
        QByteArray recentBooksData;
        if(recentBooksDatabase.open(QIODevice::ReadOnly)) {
            recentBooksData = recentBooksDatabase.readAll();
            recentBooksDatabase.close();
        }
        else {
            QString function = __func__; log(function + ": Failed to open recent books database file for reading at '" + recentBooksDatabase.fileName() + "'", className);
        }
        QJsonObject mainJsonObject = QJsonDocument::fromJson(qUncompress(QByteArray::fromBase64(recentBooksData))).object();
        for(int i = 1; i <= global::homePageWidget::recentBooksNumber; i++) {
            QString objectName = "Book" + QString::number(i);
            QJsonObject jsonObject = mainJsonObject[objectName].toObject();
            if(i == 1) {
                if(jsonObject.value("BookPath").toString() != global::toreader::filePath) {
                    // Circular buffer
                    for(int i = global::homePageWidget::recentBooksNumber; i >= 2; i--) {
                        mainJsonObject["Book" + QString::number(i)] = mainJsonObject["Book" + QString::number(i - 1)];
                    }
                    jsonObject.insert("BookPath", QJsonValue(global::toreader::filePath));
                    mainJsonObject[objectName] = jsonObject;
                }
            }
        }
        recentBooksObject = mainJsonObject;
    }
    else {
        QJsonObject mainJsonObject;
        QJsonObject firstJsonObject;
        firstJsonObject.insert("BookPath", QJsonValue(global::toreader::filePath));
        mainJsonObject["Book1"] = firstJsonObject;

        for(int i = 2; i <= global::homePageWidget::recentBooksNumber; i++) {
            QJsonObject jsonObject;
            jsonObject.insert("BookPath", QJsonValue(""));
            mainJsonObject["Book" + QString::number(i)] = jsonObject;
        }
        recentBooksObject = mainJsonObject;
    }
    QFile::remove(global::localLibrary::recentBooksDatabasePath);
    writeFile(global::localLibrary::recentBooksDatabasePath, qCompress(QJsonDocument(recentBooksObject).toJson()).toBase64());

    batteryWatchdog(this);

    // Needed
    ui->gridLayout->setVerticalSpacing(0);

    // TODO
    ui->graphicsView->hide();

    QString tmpPage = "/inkbox/book/split/" + QString::number(currentPage);
    // Important timing
    QTimer::singleShot(20, this, [this, tmpPage] () {toreader::setText(tmpPage); });
    log("Done showing GUI", className);
}

toreader::~toreader()
{
    delete ui;
}

void toreader::setText(QString pathProvided) {
    log("SetText called", className);
    global::toreader::FileReadyMutex.lock();
    if(global::toreader::fileReady == false) {
        global::toreader::FileReadyMutex.unlock();
        log("File " + pathProvided + " doesn't exist, waiting", className);
        // Lambda curse, idk other things didn't worked
        // https://forum.qt.io/topic/73714/qtimer-singleshot-forward-parameter-to-slot-called/9
        // https://stackoverflow.com/questions/38595834/compilation-error-this-cannot-be-implicitly-captured-in-this-context
        QTimer::singleShot(30, this, [pathProvided, this] () {toreader::setText(pathProvided); });
    }
    else {
        global::toreader::FileReadyMutex.unlock();
        if(QFile{pathProvided}.exists() == false) {
            log("WARNING: Page file should exist, but it doesn't", className);
        }
        else {
            global::toreader::fileReady = false;
            log("File found", className);
            QString htmlCode = readFile(pathProvided);
            //log("Pure HTML code: \n" + htmlCode + "\n", className);

            // libreader-rs
            // TODO: WARNING: Possible memory leak, if qt doesn't manage qstring that well
            // more info:
            // https://github.com/Szybet/libreader-rs/blob/0e65478200ec02487eb081637b63ac18e73c242e/src/lib.rs#L145

            htmlCode = add_spaces(htmlCode.toStdString().c_str());
            //log("HTML code after adding spaces: \n" + htmlCode + "\n", className);

            htmlCode = cut_off_head(htmlCode.toStdString().c_str());
            //log("HTML code after cutting off head: \n" + htmlCode + "\n", className);

            // Yea i got problems with that too, needed logs
            log("Setting text", className);
            ui->text->setHtml(htmlCode);
            log("Setted text", className);
        }
    }
}

void toreader::on_previousBtn_clicked()
{

}

void toreader::on_nextBtn_clicked()
{
    currentPage = currentPage + 1;
    QTimer::singleShot(0, this, [this] () {toreader::emitRequestPageFun(currentPage); });
    setText("/inkbox/book/split/" + QString::number(currentPage));
}

void toreader::emitRequestPageFun(int page) {
    emit requestPage(page);
}

void toreader::mainSetStyle() {
    // Stylesheet things
    QFile stylesheetFile("/mnt/onboard/.adds/inkbox/eink.qss");
    stylesheetFile.open(QFile::ReadOnly);
    this->setStyleSheet(stylesheetFile.readAll());
    stylesheetFile.close();
    ui->bookInfoLabel->setStyleSheet("font-style: italic");
    ui->text->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    iconsSizeSet();
    hideThings();
    nightmode();
    setFonts();

    // Device specific

    // On the Mini with QT_FONT_DPI set to 187 (default for this device), quitBtn makes the UI go beyond the limits of the screen when the menu bar is shown
    if(global::deviceID == "n705\n") {
        ui->quitBtn->hide();
        ui->quitBtn->deleteLater();
        ui->line_19->hide();
        ui->line_19->deleteLater();
    }}

void toreader::iconsSizeSet() {
    // Elements
    ui->brightnessStatus->setFont(QFont("u001"));
    ui->fontLabel->setFont(QFont("u001"));
    ui->sizeLabel->setFont(QFont("u001"));
    ui->sizeValueLabel->setFont(QFont("Inter"));
    ui->lineSpacingLabel->setFont(QFont("u001"));
    ui->lineSpacingValueLabel->setFont(QFont("Inter"));
    ui->marginsLabel->setFont(QFont("u001"));
    ui->marginsValueLabel->setFont(QFont("Inter"));
    ui->alignmentLabel->setFont(QFont("u001"));
    ui->fontChooser->setFont(QFont("u001"));
    ui->definitionStatusLabel->setFont(QFont("u001"));
    ui->pageProgressBar->setFont(QFont("u001"));

    ui->previousBtn->setProperty("type", "borderless");
    ui->nextBtn->setProperty("type", "borderless");
    ui->optionsBtn->setProperty("type", "borderless");
    ui->brightnessDecBtn->setProperty("type", "borderless");
    ui->brightnessIncBtn->setProperty("type", "borderless");
    ui->homeBtn->setProperty("type", "borderless");
    ui->aboutBtn->setProperty("type", "borderless");
    ui->alignLeftBtn->setProperty("type", "borderless");
    ui->alignRightBtn->setProperty("type", "borderless");
    ui->alignCenterBtn->setProperty("type", "borderless");
    ui->alignLeftBtn->setProperty("type", "borderless");
    ui->alignJustifyBtn->setProperty("type", "borderless");
    ui->infoCloseBtn->setProperty("type", "borderless");
    ui->saveWordBtn->setProperty("type", "borderless");
    ui->previousDefinitionBtn->setProperty("type", "borderless");
    ui->nextDefinitionBtn->setProperty("type", "borderless");
    ui->nightModeBtn->setProperty("type", "borderless");
    ui->searchBtn->setProperty("type", "borderless");
    ui->gotoBtn->setProperty("type", "borderless");
    ui->increaseScaleBtn->setProperty("type", "borderless");
    ui->decreaseScaleBtn->setProperty("type", "borderless");
    ui->quitBtn->setProperty("type", "borderless");
    ui->viewHighlightsBtn->setProperty("type", "borderless");

    // Icons
    ui->alignLeftBtn->setText("");
    ui->alignLeftBtn->setIcon(QIcon(":/resources/align-left.png"));
    ui->alignRightBtn->setText("");
    ui->alignRightBtn->setIcon(QIcon(":/resources/align-right.png"));
    ui->alignCenterBtn->setText("");
    ui->alignCenterBtn->setIcon(QIcon(":/resources/align-center.png"));
    ui->alignJustifyBtn->setText("");
    ui->alignJustifyBtn->setIcon(QIcon(":/resources/align-justify.png"));
    ui->infoCloseBtn->setText("");
    ui->infoCloseBtn->setIcon(QIcon(":/resources/close.png"));
    ui->saveWordBtn->setText("");
    ui->saveWordBtn->setIcon(QIcon(":/resources/star.png"));
    ui->previousDefinitionBtn->setText("");
    ui->previousDefinitionBtn->setIcon(QIcon(":/resources/chevron-left.png"));
    ui->nextDefinitionBtn->setText("");
    ui->nextDefinitionBtn->setIcon(QIcon(":/resources/chevron-right.png"));
    ui->brightnessDecBtn->setText("");
    ui->brightnessDecBtn->setIcon(QIcon(":/resources/minus.png"));
    ui->brightnessIncBtn->setText("");
    ui->brightnessIncBtn->setIcon(QIcon(":/resources/plus.png"));
    ui->homeBtn->setText("");
    ui->homeBtn->setIcon(QIcon(":/resources/home.png"));
    ui->aboutBtn->setText("");
    ui->aboutBtn->setIcon(QIcon(":/resources/info.png"));
    ui->searchBtn->setText("");
    ui->searchBtn->setIcon(QIcon(":/resources/search.png"));
    ui->increaseScaleBtn->setText("");
    ui->increaseScaleBtn->setIcon(QIcon(":/resources/zoom-in.png"));
    ui->decreaseScaleBtn->setText("");
    ui->decreaseScaleBtn->setIcon(QIcon(":/resources/zoom-out.png"));
    ui->quitBtn->setText("");
    ui->quitBtn->setIcon(QIcon(":/resources/power.png"));
    ui->previousBtn->setText("");
    ui->previousBtn->setIcon(QIcon(":/resources/arrow-left.png"));
    ui->optionsBtn->setText("");
    ui->optionsBtn->setIcon(QIcon(":/resources/settings.png"));
    ui->nextBtn->setText("");
    ui->nextBtn->setIcon(QIcon(":/resources/arrow-right.png"));
    ui->viewHighlightsBtn->setText("");
    ui->viewHighlightsBtn->setIcon(QIcon(":/resources/view-highlights.png"));

    // Defining pixmaps
    // Getting the screen's size
    float sW = QGuiApplication::screens()[0]->size().width();
    float sH = QGuiApplication::screens()[0]->size().height();
    // Defining what the icons' size will be
    if(checkconfig("/opt/inkbox_genuine") == true) {
        float stdIconWidth;
        float stdIconHeight;
        if(global::deviceID == "n705\n" or global::deviceID == "n905\n" or global::deviceID == "n613\n" or global::deviceID == "n236\n" or global::deviceID == "n437\n" or global::deviceID == "n306\n" or global::deviceID == "kt\n" or global::deviceID == "emu\n") {
            stdIconWidth = sW / 16;
            stdIconHeight = sW / 16;
        }
        else {
            stdIconWidth = sW / 19;
            stdIconHeight = sH / 19;
        }
        QPixmap chargingPixmap(":/resources/battery_charging.png");
        scaledChargingPixmap = chargingPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QPixmap fullPixmap(":/resources/battery_full.png");
        scaledFullPixmap = fullPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QPixmap halfPixmap(":/resources/battery_half.png");
        scaledHalfPixmap = halfPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QPixmap emptyPixmap(":/resources/battery_empty.png");
        scaledEmptyPixmap = emptyPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else {
        float stdIconWidth = sW / 19;
        float stdIconHeight = sH / 19;
        QPixmap chargingPixmap(":/resources/battery_charging.png");
        scaledChargingPixmap = chargingPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio);
        QPixmap fullPixmap(":/resources/battery_full.png");
        scaledFullPixmap = fullPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio);
        QPixmap halfPixmap(":/resources/battery_half.png");
        scaledHalfPixmap = halfPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio);
        QPixmap emptyPixmap(":/resources/battery_empty.png");
        scaledEmptyPixmap = emptyPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio);
    }
}

void toreader::setFonts() {
    global::reader::font = readFile(".config/04-book/font");
    if(global::reader::font == "u001") {
        ui->fontChooser->setCurrentText("Univers (u001)");
    }
    else {
        ui->fontChooser->setCurrentText(global::reader::font);
    }

    if(global::deviceID == "n873\n") {
        ui->nextBtn->setStyleSheet("padding: 13.5px");
        ui->previousBtn->setStyleSheet("padding: 13.5px");
        ui->optionsBtn->setStyleSheet("padding: 13.5px");
    }
    else if(global::deviceID == "n437\n") {
        ui->nextBtn->setStyleSheet("padding: 12.5px");
        ui->previousBtn->setStyleSheet("padding: 12.5px");
        ui->optionsBtn->setStyleSheet("padding: 12.5px");
    }
    else {
        ui->nextBtn->setStyleSheet("padding: 10px");
        ui->previousBtn->setStyleSheet("padding: 10px");
        ui->optionsBtn->setStyleSheet("padding: 10px");
    }
    ui->sizeValueLabel->setStyleSheet("font-size: 9pt; font-weight: bold");
    ui->lineSpacingValueLabel->setStyleSheet("font-size: 9pt; font-weight: bold");
    ui->marginsValueLabel->setStyleSheet("font-size: 9pt; font-weight: bold");
    ui->homeBtn->setStyleSheet("font-size: 9pt; padding: 5px");
    ui->aboutBtn->setStyleSheet("font-size: 9pt; padding: 5px");
    ui->fontChooser->setStyleSheet("font-size: 9pt");
    ui->gotoBtn->setStyleSheet("font-size: 9pt; padding: 9px; font-weight: bold; background: lightGrey");
    ui->pageNumberLabel->setFont(QFont("Source Serif Pro"));
    ui->viewHighlightsBtn->setStyleSheet("padding: 9px");


    int id = QFontDatabase::addApplicationFont(":/resources/fonts/CrimsonPro-Italic.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont crimson(family);
    ui->bookInfoLabel->setFont(crimson);
}

void toreader::nightmode() {
    // Night mode
    if(global::deviceID == "n705\n" or global::deviceID == "n905\n" or global::deviceID == "n613\n" or global::deviceID == "n236\n" or global::deviceID == "n437\n" or global::deviceID == "n306\n") {
        if(checkconfig(".config/10-dark_mode/config") == true) {
            log("Setting night mode to ON", className);
            string_writeconfig("/tmp/invertScreen", "y");
            ui->nightModeBtn->setText("");
            ui->nightModeBtn->setIcon(QIcon(":/resources/nightmode-full.png"));
            isNightModeActive = true;
        }
        else {
            log("Setting night mode to OFF", className);
            string_writeconfig("/tmp/invertScreen", "n");
            ui->nightModeBtn->setText("");
            ui->nightModeBtn->setIcon(QIcon(":/resources/nightmode-empty.png"));
            isNightModeActive = false;
        }
    }
    else {
        log("Night mode disabled by software", className);
        ui->line_7->hide();
        ui->line_7->deleteLater();
        ui->nightModeBtn->hide();
        ui->nightModeBtn->deleteLater();
    }
}

void toreader::hideThings() {
    // Hiding the menubar + definition widget + brightness widget + buttons bar widget
    ui->menuWidget->setVisible(false);
    ui->brightnessWidget->setVisible(false);
    ui->menuBarWidget->setVisible(false);
    ui->buttonsBarWidget->setVisible(false);
    ui->pdfScaleWidget->setVisible(false);
    ui->wordWidget->setVisible(false);
    if(checkconfig(".config/11-menubar/sticky") == true) {
        ui->menuWidget->setVisible(true);
        ui->statusBarWidget->setVisible(true);
    }
    else {
        ui->menuWidget->setVisible(false);
        ui->statusBarWidget->setVisible(false);
    }
    ui->pageWidget->hide();

    //showTopbarWidget = true;
    // Topbar widget / book info
    ui->topbarStackedWidget->setVisible(true);
}
