#ifndef MUPDFCALLER_H
#define MUPDFCALLER_H

#include "mupdf/fitz.h"

#include <stdlib.h>
#include <stdio.h>

#include <QString>

// https://mupdf.readthedocs.io/en/latest/mutool-convert.html
// Default for epub:
// inhibit-spacespreserve-whitespace,dehyphenate,preserve-images
// This is a full file path

// This is weird: https://github.com/Kobo-InkBox/mupdf/blob/9e6ab0cbb829926dd7147a9b5b42bd1a32ca96d7/mupdf/source/fitz/stext-device.c#L920
// mupdf/source/fitz/stext-device.c at line 920
// Undocumented feature, diffrent syntax?
// Not usefull for us, but the math is interesting? nope it only scales in a weird way
void initMupdf(int width, int height, int fontSizeInPoints, QString outputOptions, QString outputFormat, QString filePath);
void deInitMupdf();
void tryReflowMuPdf(int width, int height, int fontSizeInPoints);
void calculatePagesCount();
void clearMemory();

// This is the real number, not starting from 0
// Yes, you need to clean the data after!
QByteArray* getPageData(int number);

#endif // MUPDFCALLER_H
