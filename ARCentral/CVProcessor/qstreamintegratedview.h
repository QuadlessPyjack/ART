#ifndef QSTREAMINTEGRATEDVIEW_H
#define QSTREAMINTEGRATEDVIEW_H

#include "frameprocessor.h"

#include <QQuickItem>

#include <QQuickPaintedItem>
#include <QImage>
#include <QPainter>
#include <QTime>

class QStreamIntegratedView : public QQuickPaintedItem
{
    Q_OBJECT
public:
    QStreamIntegratedView(QQuickItem *parent = nullptr);
    Q_INVOKABLE void updateImage(const QImage &updated);

signals:

public slots:
private:
    QImage m_currentImage;
    FrameProcessor m_frameProcessor;
    void paint(QPainter *painter);
    int m_frameCounter = 0;
    QTime m_time;
};

#endif // QSTREAMINTEGRATEDVIEW_H
