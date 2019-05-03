#include <QList>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "frameprocessor.h"
#include "utils.h"

FrameProcessor::FrameProcessor(QObject *parent) : QObject(parent)
  , m_CVCaptureDevice(m_StreamUrl) //m_StreamUrl
{
    loadBaseImage();
}

void FrameProcessor::loadBaseImage()
{
    m_patternSolver.LoadBaseImage("./Data/DSC_0774.JPG");
}

void FrameProcessor::generateCalibrationMarkers(bool b)
{
    DataPoint c1, c2, c3, c4;
    c1.X = 0;
    c1.Y = 0;
    c1.type = TARGET;
    c1.IsVisible = true;
    c2.X = 120;
    c2.Y = 0;
    c2.type = TARGET;
    c2.IsVisible = true;
    c3.X = 0;
    c3.Y = 56;
    c3.type = TARGET;
    c3.IsVisible = true;
    c4.X = 120;
    c4.Y = 56;
    c4.type = TARGET;
    c4.IsVisible = true;

    if (b == true)
    {
        m_qMgr.AddToQueue(c1);
        m_qMgr.AddToQueue(c2);
    } else
    {
        m_qMgr.AddToQueue(c3);
        m_qMgr.AddToQueue(c4);
    }
}

void FrameProcessor::processFrame()
{
    if ( m_CVCaptureDevice.isOpened() )
    {
        m_CVCaptureDevice >> m_CVframe;
        QList<DataPoint> markerPoints;
        QImage processed = m_patternSolver.processFrame(m_CVframe, markerPoints);

        emit imageProcessed(processed);
        //b = !b;
        //generateCalibrationMarkers(b);

        if (markerPoints.count() == 0)
        {
            return;
        }

        if (prevT.type == -1 && prevM.type == -1)
        {
            prevT = markerPoints[0];
            prevM = markerPoints[1];

            m_qMgr.AddToQueue(prevT);
            m_qMgr.AddToQueue(prevM);
        }

        for (int dpIdx = 0; dpIdx < markerPoints.count(); ++dpIdx)
        {
              DataPoint point = markerPoints[dpIdx];
//            DataPoint dp;
//            dp.X    = 0b00000001;
//            dp.Y    = 0b00000010;
//            dp.Z    = 0b00000000;
//            dp.type = 0b00000100;

            // don't flood the transmission with duplicate values
            if ((dpIdx % 2) == 0)
            {
                if (point == prevT)
                {
                    // target will always remain at the same coordinates unless there's a change in marker type
                    //continue;
                }

                prevT = point;
            } else {
                if (point == prevM)
                {
                    continue;
                }

                prevM = point;
            }

            m_qMgr.AddToQueue(point);
            qDebug() << "[Point DTO] Type: " << point.type <<  "X: " << point.X << " Y: " << point.Y << " Z: " << point.Z;
        }

    }
}
