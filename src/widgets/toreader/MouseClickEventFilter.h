#ifndef MOUSECLICKEVENTFILTER_H
#define MOUSECLICKEVENTFILTER_H


#include "qevent.h"
#include <QObject>
#include <QEvent>
#include <QDebug>

class MouseClickEventFilter : public QObject
{
    Q_OBJECT

public:
    explicit MouseClickEventFilter(QObject *parent = nullptr) : QObject(parent) {}

signals:
    void Clicked(QPoint point);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override
    {
        if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            qDebug() << "Mouse click at" << mouseEvent->pos();
            emit Clicked(mouseEvent->pos());
        }

        return QObject::eventFilter(obj, event);
    }
};

#endif // MOUSECLICKEVENTFILTER_H
