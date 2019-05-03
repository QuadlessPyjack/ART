#ifndef FRAMEPROCESSOR_H
#define FRAMEPROCESSOR_H

#include "patternsolver.h"
#include <blequeuemanager.h>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;

#include <QObject>

class FrameProcessor : public QObject
{
    Q_OBJECT
public:
    explicit FrameProcessor(QObject *parent = nullptr);
    void loadBaseImage();
    Q_INVOKABLE void processFrame();

    void generateCalibrationMarkers(bool b);
    bool b = true;
signals:
    void imageProcessed(const QImage &image);

private:
    PatternSolver m_patternSolver;
    Mat m_CVframe;
    const String m_StreamUrl = "http://admin:@10.0.1.250/video.cgi?resolution=VGA";
    VideoCapture m_CVCaptureDevice;
    BLEQueueManager m_qMgr;
    DataPoint prevT;
    DataPoint prevM;
};

#endif // FRAMEPROCESSOR_H
