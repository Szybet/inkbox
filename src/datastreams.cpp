#include "functions.h"
#include "datastreams.h"
#include <QDataStream>

// Use this file for qDatastreams decklarations, which are needed for QSettings

QDataStream& operator<<(QDataStream& out, const global::toreader::toreaderConfig& v) {
    out << v.savedPage;
    out << v.format;
    out << v.usesImageEngine;
    out << v.isSingleImage;
    out << v.font;
    out << v.fontSize;
    out << v.lineSpacing;
    out << v.margins;
    out << v.alignment;
    out << v.brightness;
    out << v.brightnessWarmth;
    out << v.width;
    out << v.height;
    out << v.highlighting;
    out << v.loadHighlightsSlow;
    out << v.progress;
    out << v.pagesCount;
    out << v.pagesCountSlow;
    out << v.preCachedPages;
    out << v.imageAdjust;
    out << v.skipEmptyPages;
    out << v.buttonsEnabled;
    out << v.additionallInfo;
    return out;
}

QDataStream& operator>>(QDataStream& in, global::toreader::toreaderConfig& v) {
    in >> v.savedPage;
    in >> v.format;
    in >> v.usesImageEngine;
    in >> v.isSingleImage;
    in >> v.font;
    in >> v.fontSize;
    in >> v.lineSpacing;
    in >> v.margins;
    in >> v.alignment;
    in >> v.brightness;
    in >> v.brightnessWarmth;
    in >> v.width;
    in >> v.height;
    in >> v.highlighting;
    in >> v.loadHighlightsSlow;
    in >> v.progress;
    in >> v.pagesCount;
    in >> v.pagesCountSlow;
    in >> v.preCachedPages;
    in >> v.imageAdjust;
    in >> v.skipEmptyPages;
    in >> v.buttonsEnabled;
    in >> v.additionallInfo;
    return in;
}

QDataStream& operator<<(QDataStream& out, const global::translate::translateConfig& v) {
    out << v.url;
    out << v.apiKey;
    out << v.langFrom;
    out << v.langTo;
    return out;
}

QDataStream& operator>>(QDataStream& in, global::translate::translateConfig& v) {
    in >> v.url;
    in >> v.apiKey;
    in >> v.langFrom;
    in >> v.langTo;
    return in;
}


// Chatgpt is awesome for writing this crap
QDebug operator<<(QDebug dbg, const global::toreader::toreaderConfig& v) {
    dbg.nospace() << "toreaderConfig(savedPage=" << v.savedPage << ", format=" << v.format << ", usesImageEngine=" << v.usesImageEngine << ", isSingleImage=" << v.isSingleImage << ", font=" << v.font
                  << ", fontSize=" << v.fontSize << ", lineSpacing=" << v.lineSpacing << ", margins=" << v.margins
                  << ", alignment=" << v.alignment << ", brightness=" << v.brightness << ", brightnessWarmth=" << v.brightnessWarmth
                  << ", width=" << v.width << ", height=" << v.height << ", highlighting=" << v.highlighting << ", loadHighlightsSlow=" << v.loadHighlightsSlow
                  << ", progress=" << v.progress << ", pagesCount=" << v.pagesCount << ", pagesCountSlow=" << v.pagesCountSlow
                  << ", preCachedPages=" << v.preCachedPages << ", imageAdjust=" << v.imageAdjust << ", skipEmptyPages=" << v.skipEmptyPages
                  << ", buttonsEnabled=" << v.buttonsEnabled << ", additionallInfo=" << v.additionallInfo << ")";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const global::translate::translateConfig& v) {
    dbg.nospace() << "translateConfig("
                  << "url: " << v.url
                  << ", apiKey: " << v.apiKey
                  << ", langFrom: " << v.langFrom
                  << ", langTo: " << v.langTo
                  << ")";
    return dbg;
}

void declareMetaTypes() {
    // First this
    qRegisterMetaTypeStreamOperators<global::toreader::toreaderConfig>("global::toreader::toreaderConfig");
    qRegisterMetaTypeStreamOperators<global::translate::translateConfig>("global::translate::translateConfig");

    // Then this
    qRegisterMetaType<global::toreader::toreaderConfig>("global::toreader::toreaderConfig");
    qRegisterMetaType<global::translate::translateConfig>("global::translate::translateConfig");
}
