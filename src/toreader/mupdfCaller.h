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
void initMupdf(int width, int height, int fontSizeInPoints, QString outputOptions, QString outputFormat, QString filePath);
void deInitMupdf();
// This is the real number, not starting from 0
// Yes, you need to clean the data after!
QByteArray* getPageData(int number);

#endif // MUPDFCALLER_H
