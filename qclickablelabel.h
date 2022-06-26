#ifndef QCLICKABLELABEL_H
#define QCLICKABLELABEL_H

#include <QLabel>
#include <QWidget>
#include <Qt>

class QClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit QClickableLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~QClickableLabel();

signals:
    void clicked();
    void unclicked();
    void bookID(int id);

protected:
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);

};

#endif // CLICKABLELABEL_H
