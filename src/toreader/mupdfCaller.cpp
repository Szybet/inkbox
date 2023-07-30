#include "mupdfCaller.h"

#include <QDebug>

void initMupdf(int width, int height, int fontSizeInPoints, QString outputOptions, QString outputFormat, QString filePath) {
    layout_w = width;
    layout_h = height;
    layout_em = fontSizeInPoints;
    options = outputOptions.toStdString().c_str();
    format = outputFormat.toStdString().c_str();

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

    fz_set_aa_level(ctx, alphabits);
    fz_set_use_document_css(ctx, layout_use_doc_css);

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

    doc = fz_open_document(ctx, filePath.toStdString().c_str());
    fz_layout_document(ctx, doc, layout_w, layout_h, layout_em);
    // This can take time?
    qDebug() << "Start counting pages";
    int countPages = fz_count_pages(ctx, doc);
    qDebug() << "Counting pages (and mupdf init) finished :" << countPages;
}

void deInitMupdf() {
    fz_drop_document(ctx, doc);
    doc = NULL;
    fz_drop_context(ctx);
}

// Not completed
void getPage(int number) {
    fz_rect mediabox;
    fz_page *page;
    fz_device *dev = NULL;

    page = fz_load_page(ctx, doc, number - 1);

    fz_var(dev);

    fz_try(ctx)
    {
        mediabox = fz_bound_page(ctx, page);
        dev = fz_begin_page(ctx, outputwriter, mediabox);
        fz_run_page(ctx, page, dev, fz_identity, NULL);
        fz_end_page(ctx, outputwriter);
    }
    fz_always(ctx)
    {
        fz_drop_page(ctx, page);
    }
    fz_catch(ctx)
        fz_rethrow(ctx);
}
