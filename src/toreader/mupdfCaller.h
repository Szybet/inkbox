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
void getPage(int number);

/* input options */
static int alphabits = 8;
static float layout_w = FZ_DEFAULT_LAYOUT_W;
static float layout_h = FZ_DEFAULT_LAYOUT_H;
static float layout_em = FZ_DEFAULT_LAYOUT_EM;
static char *layout_css = NULL;
static int layout_use_doc_css = 1;

/* output options */
static const char *format = NULL;
static const char *options = "";

static fz_context *ctx;
static fz_document *doc;

#endif // MUPDFCALLER_H
