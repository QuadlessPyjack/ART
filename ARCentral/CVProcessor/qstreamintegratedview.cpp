#include "qstreamintegratedview.h"

QStreamIntegratedView::QStreamIntegratedView(QQuickItem *parent)
    : QQuickPaintedItem (parent)
{
    connect(&m_frameProcessor, &FrameProcessor::imageProcessed,
            this, &QStreamIntegratedView::updateImage);

    m_frameProcessor.processFrame();
}

void QStreamIntegratedView::paint(QPainter *painter)
{
    int currentFPS = 0;
    if (m_frameCounter == 0)
    {
        m_time.start();
    } else {
        currentFPS = m_time.elapsed() / static_cast<float>(m_frameCounter);
    }
    m_frameCounter++;

    if (m_currentImage.isNull())
    {
        qDebug() << "Stream loading or not available";
        painter->drawText(0, 0, "Stream loading or not available");
    }

    QSizeF scaled = QSizeF(m_currentImage.width(),
                           m_currentImage.height())
            .scaled(boundingRect().size(), Qt::KeepAspectRatio);
    QRect centreRect(static_cast<int>(qAbs(scaled.width() - width()) / 2.0f),
                     static_cast<int>(qAbs(scaled.height() - height()) / 2.0f),
                     static_cast<int>(scaled.width()),
                     static_cast<int>(scaled.height()));
    painter->drawImage(centreRect, m_currentImage);
    QString FPSCaption = QString("FPS: %1").arg(QString::number(currentFPS));
    painter->drawText(20, 20, FPSCaption);

    m_frameProcessor.processFrame();
}

void QStreamIntegratedView::updateImage(const QImage &updated)
{
    m_currentImage = updated;
    update();
}
