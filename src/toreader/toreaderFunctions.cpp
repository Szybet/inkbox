#include "toreaderFunctions.h"
#include "build/.ui/ui_toreader.h"
#include "toreader.h"
#include "QScreen"
#include "toast.h"
#include "mupdfCaller.h"

#include <QSettings>
#include <QFontDatabase>
#include <QCryptographicHash>

Ui::toreader *ui = NULL;
toreader* thiss = NULL;
global::toreader::toreaderConfig* conf;

void initVarsForFun(Ui::toreader *uiArg, toreader* thissArg, global::toreader::toreaderConfig* confArg) {
    ui = uiArg;
    thiss = thissArg;
    conf = confArg;
}

QSettings *settings;
void loadConfig() {
    QFile file(global::toreader::filePath);
    // add together 3k bytes at the beginning, middle, end of the file and make a md5sum of them
    if(file.open(QIODevice::ReadOnly | QIODevice::Unbuffered) == false) {
        qDebug() << "Failed to open this book" << file;
        exit(-1);
    }
    quint64 size = file.size();
    qDebug() << "The size of the book:" << size;
    file.seek(0);
    QByteArray bytes = file.read(3000);
    file.seek(size / 2);
    bytes.append(file.read(3000));
    file.seek(size - 3000);
    bytes.append(file.read(3000));
    file.close();
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(bytes);
    QString md5sum = QString::fromLocal8Bit(hash.result().toHex()); // Yes, to Hex
    bytes.clear();
    qDebug() << "The md5sum of the book:" << md5sum;

    global::toreader::configFilePath = "/mnt/onboard/onboard/.progress/" + md5sum + "/config";
    QFile fileConfig(global::toreader::configFilePath);
    if(fileConfig.exists() == false) {
        // Create the dir
        {
            QDir dir = QFileInfo(fileConfig).dir();
            if (dir.mkpath(dir.absolutePath()) == false) {
                qDebug() << "Failed to create config dir";
            }
        }
        // Load default config
        // Replace this path with something global in the future
        QString defaultConfigPath = "/mnt/onboard/.adds/inkbox/.config/settings.ini";
        QFile defaultConfig(defaultConfigPath);
        // It doesn't exist :(
        if(defaultConfig.exists() == false) {
            QDir dir = QFileInfo(fileConfig).dir();
            if (dir.mkpath(dir.absolutePath()) == false) {
                qDebug() << "Failed to create main default config dir";
            }
            QSettings *settingsGlobal = new QSettings(defaultConfig.fileName(), QSettings::IniFormat);
            QVariant variant = QVariant::fromValue(*conf);
            settingsGlobal->setValue("toreader", variant);
            settingsGlobal->sync();
            delete settingsGlobal;
        }
        QFile::copy(defaultConfigPath, global::toreader::configFilePath);
    }
    settings = new QSettings(global::toreader::configFilePath, QSettings::IniFormat);
    settings->setParent(thiss);
    *conf = settings->value("toreader").value<global::toreader::toreaderConfig>();
    qDebug() << "Loaded config:" << *conf;

    if(conf->format.isEmpty()) {
        QString format = global::toreader::filePath.split(".").last();
        qDebug() << "The format is:" << format;
        if(global::toreader::supportedFormats.contains(format)) {
            conf->format = format;
            if(format == "pdf") {
                conf->usesImageEngine = true;
            }
        }
        else {
            if(global::toreader::supportedFormatsImages.contains(format)) {
                conf->format = "image";
                conf->usesImageEngine = true;
                conf->isSingleImage = true;
            }
            else {
                qDebug() << "Format is wrong";
                exit(-1);
            }
        }
    }
}
void saveConfig() {
    // todo
}

// Battery icons?
// There is an error about qguiapp and pixmal, tldr it needs to be a reference
QPixmap* scaledChargingPixmap;
QPixmap* scaledHalfPixmap;
QPixmap* scaledFullPixmap;
QPixmap* scaledEmptyPixmap;

void batteryWatchdog() {
    // Battery watchdog
    if(global::reader::startBatteryWatchdog == true) {
        QTimer *t = new QTimer(thiss);
        t->setInterval(2000);
        QObject::connect(t, &QTimer::timeout, [&]() {
            // Checking if battery level is low
            if(global::battery::showCriticalBatteryAlert != true) {
                ;
            }
            else {
                if(isBatteryCritical() == true) {
                    qDebug() << "Warning! Battery is at a critical charge level!";
                    //openCriticalBatteryAlertWindow(); // TODO
                }
            }

            if(global::battery::showLowBatteryDialog != true) {
                // Do nothing, since a dialog should already have been displayed and (probably) dismissed
                ;
            }
            else {
                if(isBatteryLow() == true) {
                    if(global::battery::batteryAlertLock == true) {
                        ;
                    }
                    else {
                        if(isUsbPluggedIn()) {
                            ;
                        }
                        else {
                            qDebug() << "Warning! Battery is low!";
                            //openLowBatteryDialog(); // TODO
                        }
                    }
                }
            }
        } );
        if(global::deviceID != "emu\n") {
            t->start();
        }
    }
}

void mainSetStyle() {
    qDebug() << "Setting styles";
    // Stylesheet things
    QFile stylesheetFile("/mnt/onboard/.adds/inkbox/eink.qss");
    stylesheetFile.open(QFile::ReadOnly);
    thiss->setStyleSheet(stylesheetFile.readAll());
    stylesheetFile.close();
    ui->bookInfoLabel->setStyleSheet("font-style: italic");
    ui->text->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->text->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    qDebug() << "Setting other styles";
    iconsSizeSet();
    hideThings();

    if(global::deviceID == "n873\n") {
        ui->nextBtn->setStyleSheet("padding: 13.5px");
        ui->previousBtn->setStyleSheet("padding: 13.5px");
        ui->optionsBtn->setStyleSheet("padding: 13.5px");
    }
    else if(global::deviceID == "n437\n" or global::deviceID == "n249\n") {
        ui->nextBtn->setStyleSheet("padding: 12.5px");
        ui->previousBtn->setStyleSheet("padding: 12.5px");
        ui->optionsBtn->setStyleSheet("padding: 12.5px");
    }
    else {
        ui->nextBtn->setStyleSheet("padding: 10px");
        ui->previousBtn->setStyleSheet("padding: 10px");
        ui->optionsBtn->setStyleSheet("padding: 10px");
    }
    ui->lineSpacingValueLabel->setStyleSheet("font-size: 9pt; font-weight: bold");
    ui->homeBtn->setStyleSheet("font-size: 9pt; padding: 5px");
    ui->brightnessBtn->setStyleSheet("font-size: 9pt; padding: 5px");
    ui->gotoBtn->setStyleSheet("font-size: 9pt; padding: 9px; font-weight: bold; background: lightGrey");
    ui->pageNumberLabel->setFont(QFont("Source Serif Pro"));
    ui->viewHighlightsBtn->setStyleSheet("padding: 9px");

    // Device specific
    // On the Mini with QT_FONT_DPI set to 187 (default for this device), quitBtn makes the UI go beyond the limits of the screen when the menu bar is shown
    if(global::deviceID == "n705\n") {
        ui->quitBtn->hide();
        ui->quitBtn->deleteLater();
        ui->line_19->hide();
        ui->line_19->deleteLater();
    }
}

void iconsSizeSet() {
    qDebug() << "Setting icons sizes";
    // Elements
    ui->lineSpacingLabel->setFont(QFont("u001"));
    ui->lineSpacingValueLabel->setFont(QFont("Inter"));
    ui->alignmentLabel->setFont(QFont("u001"));
    ui->pageProgressBar->setFont(QFont("u001"));
    ui->fontBtn->setFont(QFont("u001", 10));

    ui->previousBtn->setProperty("type", "borderless");
    ui->nextBtn->setProperty("type", "borderless");
    ui->optionsBtn->setProperty("type", "borderless");
    ui->homeBtn->setProperty("type", "borderless");
    ui->alignLeftBtn->setProperty("type", "borderless");
    ui->alignRightBtn->setProperty("type", "borderless");
    ui->alignCenterBtn->setProperty("type", "borderless");
    ui->alignLeftBtn->setProperty("type", "borderless");
    ui->alignJustifyBtn->setProperty("type", "borderless");
    //ui->nightModeBtn->setProperty("type", "borderless");
    ui->searchBtn->setProperty("type", "borderless");
    ui->gotoBtn->setProperty("type", "borderless");
    ui->increaseScaleBtn->setProperty("type", "borderless");
    ui->decreaseScaleBtn->setProperty("type", "borderless");
    ui->quitBtn->setProperty("type", "borderless");
    ui->viewHighlightsBtn->setProperty("type", "borderless");
    ui->fontBtn->setProperty("type", "borderless");

    // Icons
    ui->alignLeftBtn->setText("");
    ui->alignLeftBtn->setIcon(QIcon(":/resources/align-left.png"));
    ui->alignRightBtn->setText("");
    ui->alignRightBtn->setIcon(QIcon(":/resources/align-right.png"));
    ui->alignCenterBtn->setText("");
    ui->alignCenterBtn->setIcon(QIcon(":/resources/align-center.png"));
    ui->alignJustifyBtn->setText("");
    ui->alignJustifyBtn->setIcon(QIcon(":/resources/align-justify.png"));
    ui->homeBtn->setText("");
    ui->homeBtn->setIcon(QIcon(":/resources/home.png"));
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

    ui->fontBtn->setText("");
    ui->fontBtn->setIcon(QIcon("://resources/font.png"));
    // Make a program wide standard for these.
    //ui->fontBtn->setIconSize(QSize(128, 128)); // This messes things up...


    // Defining pixmaps
    // Getting the screen's size
    float sW = QGuiApplication::screens()[0]->size().width();
    float sH = QGuiApplication::screens()[0]->size().height();
    // Defining what the icons' size will be
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
        // Memory leak let's go
        scaledChargingPixmap = new QPixmap(":/resources/battery_charging.png");
        *scaledChargingPixmap = scaledChargingPixmap->scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        scaledFullPixmap = new QPixmap(":/resources/battery_full.png");
        *scaledFullPixmap = scaledFullPixmap->scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        scaledHalfPixmap = new QPixmap(":/resources/battery_half.png");
        *scaledHalfPixmap = scaledHalfPixmap->scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        scaledEmptyPixmap = new QPixmap(":/resources/battery_empty.png");
        *scaledEmptyPixmap = scaledEmptyPixmap->scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void hideThings() {
    // Hiding the menubar + definition widget + brightness widget + buttons bar widget
    ui->menuWidget->setVisible(false);
    ui->menuBarWidget->setVisible(false);
    ui->buttonsBarWidget->setVisible(false);
    ui->pdfScaleWidget->setVisible(false);
    ui->pageWidget->hide();

    //showTopbarWidget = true;
    // Topbar widget / book info
    ui->topbarStackedWidget->setVisible(true);
}

void manageRecentBooksPages() {
    // Maintain a 'Recent books' list
    QJsonObject recentBooksObject;
    if(QFile::exists(global::localLibrary::recentBooksDatabasePath)) {
        qDebug() << "Reading recent books database";
        QFile recentBooksDatabase(global::localLibrary::recentBooksDatabasePath);
        QByteArray recentBooksData;
        if(recentBooksDatabase.open(QIODevice::ReadOnly)) {
            recentBooksData = recentBooksDatabase.readAll();
            recentBooksDatabase.close();
        }
        else {
            qDebug() << "Failed to open recent books database file for reading at" << recentBooksDatabase.fileName();
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
}

void showToast(QString messageToDisplay) {
    global::toast::message = messageToDisplay;
    toast* toastWindow = new toast(); // It doesn't need a parent, it's closed anyway
    toastWindow->show();
}

// 0 left, 1 center, 2 right, 3 justify
void setAlignment() {
    int *alignment = &conf->alignment;
    qDebug() << "Setting alignment:" << *alignment;
    if(*alignment == 0) {
        ui->text->setAlignment(Qt::AlignLeft);
    }
    else if(*alignment == 1) {
        ui->text->setAlignment(Qt::AlignHCenter);
    }
    else if(*alignment == 2) {
        ui->text->setAlignment(Qt::AlignRight);
    }
    else if(*alignment == 3) {
        ui->text->setAlignment(Qt::AlignJustify);
    }
}

int previousAlignment = -1;
void setTextStyle(QString* textProvided, bool containsImage) {
    qDebug() << "Setting style";
    if(conf->imageAdjust == true) {
        qDebug() << "Checking for image";
        if(containsImage == true) {
            qDebug() << "Image found";
            previousAlignment = conf->alignment;
            conf->alignment = 1;
            ui->text->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            ui->text->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
        else if(previousAlignment != -1){
            conf->alignment = previousAlignment;
            previousAlignment = -1;
            ui->text->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            ui->text->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }
    setAlignment();
    ui->text->setFont(conf->font);
}

bool menubarShown = false;
void openMenu() {
    if(menubarShown == true) {
        // Hiding menu bar - style code
        ui->menuWidget->setVisible(false);
        if(conf->usesImageEngine == false) {
            ui->menuBarWidget->setVisible(false);
        }
        else {
            // ?
            //if(conf->usesImageEngine == true) {
            ui->pdfScaleWidget->setVisible(false);
            //}
            //ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            //ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
        ui->buttonsBarWidget->setVisible(false);
        ui->statusBarWidget->setVisible(false);
        if(conf->isSingleImage == false) {
            ui->pageWidget->setVisible(false);
        }

        // End showing options, the rest is for the button
        if(global::deviceID == "n873\n") {
            ui->optionsBtn->setStyleSheet("background: white; color: black; padding: 13.5px");
        }
        else if(global::deviceID == "n437\n" or global::deviceID == "n249\n") {
            ui->optionsBtn->setStyleSheet("background: white; color: black; padding: 12.5px");
        }
        else {
            ui->optionsBtn->setStyleSheet("background: white; color: black; padding: 10px");
        }
        ui->optionsBtn->setIcon(QIcon(":/resources/settings.png"));
        // The Glo HD (N437) has a newer platform plugin that doesn't need this
        if(global::deviceID != "n437\n" or global::deviceID != "n306\n" or global::deviceID != "n249\n") {
            QTimer::singleShot(500, thiss, SLOT(repaint()));
        }
        menubarShown = false;
    }
    else {
        // Show menu bar
        if(isUsbPluggedIn() == true) {
            ui->batteryIconLabel->setPixmap(*scaledChargingPixmap);
        }
        else {
            getBatteryLevel();
            if(batteryLevelInt >= 75 && batteryLevelInt <= 100) {
                ui->batteryIconLabel->setPixmap(*scaledFullPixmap);
            }
            if(batteryLevelInt >= 25 && batteryLevelInt <= 74) {
                ui->batteryIconLabel->setPixmap(*scaledHalfPixmap);
            }
            if(batteryLevelInt >= 0 && batteryLevelInt <= 24) {
                ui->batteryIconLabel->setPixmap(*scaledEmptyPixmap);
            }
        }

        ui->menuWidget->setVisible(true);
        if(conf->usesImageEngine == false) {
            ui->menuBarWidget->setVisible(true);
        }
        else {
            // ?
            //if(conf->usesImageEngine == true) {
                ui->pdfScaleWidget->setVisible(true);
            //}
            ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
        ui->buttonsBarWidget->setVisible(true);
        ui->statusBarWidget->setVisible(true);
        if(conf->isSingleImage == true) {
            ui->pageWidget->setVisible(false);
        }
        else {
            ui->pageWidget->setVisible(true);
        }
        // End menu bar show
        if(global::deviceID == "n873\n") {
            ui->optionsBtn->setStyleSheet("background: black; color: white; padding: 13.5px");
        }
        else if(global::deviceID == "n437\n" or global::deviceID == "n249\n") {
            ui->optionsBtn->setStyleSheet("background: black; color: white; padding: 12.5px");
        }
        else {
            ui->optionsBtn->setStyleSheet("background: black; color: white; padding: 10px");
        }
        ui->optionsBtn->setIcon(QIcon(":/resources/settings-inverted.png"));
        //thiss->repaint();
        menubarShown = true;
    }
}

QString getPageSlowSafe(int page) {
    if(page < 0 || page > global::toreader::loadedConfig.savedPage) {
        return "";
    }
    if(global::toreader::pages[page]->isNull()) {
        QByteArray* data = getPageData(page);
        global::toreader::pages[page] = data;
        return QString::fromUtf8(*data);
    }
    else {
        return QString::fromUtf8(*global::toreader::pages[page]);
    }
}
