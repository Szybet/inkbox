#include "toreaderFunctions.h"
#include "build/.ui/ui_toreader.h"
#include "toreader.h"
#include "QScreen"

#include <QFontDatabase>

void loadConfig() {
    // todo
}
void saveConfig() {
    // todo
}

void batteryWatchdog(toreader* thisRef) {
    // Battery watchdog
    if(global::reader::startBatteryWatchdog == true) {
        QTimer *t = new QTimer(thisRef);
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

void mainSetStyle(Ui::toreader *ui, toreader* thiss) {
    // Stylesheet things
    QFile stylesheetFile("/mnt/onboard/.adds/inkbox/eink.qss");
    stylesheetFile.open(QFile::ReadOnly);
    thiss->setStyleSheet(stylesheetFile.readAll());
    stylesheetFile.close();
    ui->bookInfoLabel->setStyleSheet("font-style: italic");
    ui->text->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    iconsSizeSet(ui, thiss);
    hideThings(ui, thiss);
    setFonts(ui, thiss);

    // Device specific

    // On the Mini with QT_FONT_DPI set to 187 (default for this device), quitBtn makes the UI go beyond the limits of the screen when the menu bar is shown
    if(global::deviceID == "n705\n") {
        ui->quitBtn->hide();
        ui->quitBtn->deleteLater();
        ui->line_19->hide();
        ui->line_19->deleteLater();
    }

    // Getting brightness level
    int brightnessValue;
    if(global::isN705 == true or global::isN905C == true or global::isKT == true or global::isN873 == true) {
        brightnessValue = getBrightness();
    }
    else if(global::isN613 == true) {
        setDefaultWorkDir();
        brightnessValue = getBrightness();
    }
    else {
        brightnessValue = getBrightness();
    }
    ui->brightnessStatus->setValue(brightnessValue);
}

void iconsSizeSet(Ui::toreader *ui, toreader* thiss) {
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
    ui->alignLeftBtn->setProperty("type", "borderless");
    ui->alignRightBtn->setProperty("type", "borderless");
    ui->alignCenterBtn->setProperty("type", "borderless");
    ui->alignLeftBtn->setProperty("type", "borderless");
    ui->alignJustifyBtn->setProperty("type", "borderless");
    ui->infoCloseBtn->setProperty("type", "borderless");
    ui->saveWordBtn->setProperty("type", "borderless");
    ui->previousDefinitionBtn->setProperty("type", "borderless");
    ui->nextDefinitionBtn->setProperty("type", "borderless");
    //ui->nightModeBtn->setProperty("type", "borderless");
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
        thiss->scaledChargingPixmap = chargingPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QPixmap fullPixmap(":/resources/battery_full.png");
        thiss->scaledFullPixmap = fullPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QPixmap halfPixmap(":/resources/battery_half.png");
        thiss->scaledHalfPixmap = halfPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QPixmap emptyPixmap(":/resources/battery_empty.png");
        thiss->scaledEmptyPixmap = emptyPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else {
        float stdIconWidth = sW / 19;
        float stdIconHeight = sH / 19;
        QPixmap chargingPixmap(":/resources/battery_charging.png");
        thiss->scaledChargingPixmap = chargingPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio);
        QPixmap fullPixmap(":/resources/battery_full.png");
        thiss->scaledFullPixmap = fullPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio);
        QPixmap halfPixmap(":/resources/battery_half.png");
        thiss->scaledHalfPixmap = halfPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio);
        QPixmap emptyPixmap(":/resources/battery_empty.png");
        thiss->scaledEmptyPixmap = emptyPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio);
    }
}

void setFonts(Ui::toreader *ui, toreader* thiss) {
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
    ui->fontChooser->setStyleSheet("font-size: 9pt");
    ui->gotoBtn->setStyleSheet("font-size: 9pt; padding: 9px; font-weight: bold; background: lightGrey");
    ui->pageNumberLabel->setFont(QFont("Source Serif Pro"));
    ui->viewHighlightsBtn->setStyleSheet("padding: 9px");


    int id = QFontDatabase::addApplicationFont(":/resources/fonts/CrimsonPro-Italic.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont crimson(family);
    ui->bookInfoLabel->setFont(crimson);
}

void hideThings(Ui::toreader *ui, toreader* thiss) {
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
