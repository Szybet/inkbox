#include "mupdfCaller.h"
#include "mupdf/fitz.h"
#include "mupdf/fitz/writer.h"
#include "functions.h"

#include <QDebug>

/* input options */
static int alphabits = 8;
static float layout_w = FZ_DEFAULT_LAYOUT_W;
static float layout_h = FZ_DEFAULT_LAYOUT_H;
static float layout_em = FZ_DEFAULT_LAYOUT_EM;

/* output options */
static const char *format = NULL;
static const char *options = "";

static fz_context *ctx;
static fz_document *doc;
static fz_document_writer *out;
fz_buffer *pageBuffer;
fz_output *output;

void initMupdf(int width, int height, int fontSizeInPoints, QString outputOptions, QString outputFormat, QString filePath) {
    layout_w = width;
    layout_h = height;
    layout_em = fontSizeInPoints;
    options = outputOptions.toStdString().c_str();
    format = outputFormat.toStdString().c_str();

    qDebug() << "To replicate with mutool run:" << "mutool convert" << "-W" << width << "-H" << height << "-S" \
             << fontSizeInPoints << "-X" << "-F" << outputFormat << "-O" << outputOptions << filePath.split("/").last();

    qDebug() << "File path for the book:" << filePath;

    // Create a context to hold the exception stack and various caches.
    ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);

    if (!ctx)
    {
        qDebug() << "cannot create mupdf context";
        exit(EXIT_FAILURE);
    }

    // Register the default file types to handle.
    // Well this is a weird syntax
    fz_try(ctx)
        fz_register_document_handlers(ctx);
    fz_catch(ctx) {
        qDebug() << "cannot register document handlers:" << fz_caught_message(ctx);
        exit(EXIT_FAILURE);
    }

    qDebug() << "Setting aa levels";
    fz_set_aa_level(ctx, alphabits);
    qDebug() << "Setting document css start";
    fz_set_use_document_css(ctx, 0); // We want this like that, always
    qDebug() << "Setting document css done";

    // Propably not needed
    // Open the output document.
    /*
    fz_try(ctx)
        out = fz_new_document_writer(ctx, output, format, options);
    fz_catch(ctx)
    {
        fprintf(stderr, "cannot create document: %s\n", fz_caught_message(ctx));
        fz_drop_context(ctx);
        return EXIT_FAILURE;
    }
    */
    qDebug() << "Creating buffer";
    // 100 MB Not sure about this
    pageBuffer = fz_new_buffer(ctx, 1500000);
    qDebug() << "Creating output";
    output = fz_new_output_with_buffer(ctx, pageBuffer);
    qDebug() << "Creating document writer";
    fz_try(ctx) {
        out = fz_new_text_writer_with_output(ctx, format, output, options);
        //out = fz_new_document_writer_with_buffer(ctx, pageBuffer, format, options);
    }
    fz_catch(ctx) {
        qDebug() << "cannot create document:" << fz_caught_message(ctx);
        fz_drop_context(ctx);
        exit(EXIT_FAILURE);
    }

    qDebug() << "Opening book";
    doc = fz_open_document(ctx, filePath.toStdString().c_str());
    fz_layout_document(ctx, doc, layout_w, layout_h, layout_em);
    // This can take time? - yes it does
    qDebug() << "Start counting pages";
    if(global::toreader::loadedConfig.pagesCount == -1) {
        calculatePagesCount();
    }
    else {
        qDebug() << "Not counting pages, there is no need to";
    }
}

void deInitMupdf() {
    fz_drop_document(ctx, doc);
    fz_drop_document_writer(ctx, out);
    doc = NULL;
    fz_drop_context(ctx);
    // Remember to call it too, it's not called because of calibrate
    // clearMemory();
}

QByteArray* getPageData(int number) {
    qDebug() << "Getting page:" << number;
    fz_rect mediabox;
    fz_page *page;
    fz_device *dev = NULL;

    qDebug() << "Loading page";
    page = fz_load_page(ctx, doc, number - 1);

    qDebug() << "Creating device";
    fz_var(dev);

    QByteArray *realData = new QByteArray;

    fz_try(ctx) {
        qDebug() << "Init mediabox device";
        mediabox = fz_bound_page(ctx, page);
        qDebug() << "Begin page";
        dev = fz_begin_page(ctx, out, mediabox);
        qDebug() << "Run page";
        fz_run_page(ctx, page, dev, fz_identity, NULL);
        qDebug() << "End page";
        fz_end_page(ctx, out);
        qDebug() << "Open buffer";
        fz_stream *stream = fz_open_buffer(ctx, pageBuffer);
        qDebug() << "Calculate bytes in buffer";
        size_t bytesAvailable = fz_available(ctx, stream, 1);
        qDebug() << "Available bytes:" << bytesAvailable;
        unsigned char *data = new unsigned char[bytesAvailable];
        fz_read(ctx, stream, data, bytesAvailable);
        fz_clear_buffer(ctx, pageBuffer);

        // Convert the unsigned char* to a char* assuming it's UTF-8 encoded
        // char* chars = reinterpret_cast<char*>(data);
        // Create a std::string from the char* (assuming data is null-terminated)
        // std::string text(chars);
        // qDebug() << QString::fromStdString(text);

        realData->setRawData(reinterpret_cast<char*>(data), bytesAvailable);
        // delete[] data; // - This deleted qByteArray too
        // qDebug() << "Data to text:" << QString::fromUtf8(*realData);
    }
    fz_always(ctx) {
        fz_drop_page(ctx, page);
    }
    fz_catch(ctx) {
        fz_rethrow(ctx);
    }

    return realData;
}

void tryReflowMuPdf(int width, int height, int fontSizeInPoints) {
    fz_layout_document(ctx, doc, width, height, fontSizeInPoints);
}

void calculatePagesCount() {
    clearMemory();
    global::toreader::loadedConfig.pagesCount = fz_count_pages(ctx, doc);
    global::toreader::pages.reserve(global::toreader::loadedConfig.pagesCount);
    qDebug() << "Counting pages (and mupdf init) finished:" << global::toreader::loadedConfig.pagesCount;
}

void clearMemory() {
    for(int i = 1; i <= global::toreader::loadedConfig.pagesCount; i++) {
        if(global::toreader::pages[i]->isNull() == false) {
            global::toreader::pages[i]->clear();
        }
    }
    global::toreader::pages.clear();
}
