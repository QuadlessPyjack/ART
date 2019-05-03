#include <iostream>

#include <aruco/markerdetector.h>
#include <opencv2/opencv.hpp>

#include <QImage>
#include <QDebug>

#include "patternsolver.h"

using namespace std;

PatternSolver::PatternSolver(QObject *parent)
 : QObject(parent)
 , m_anchorAbs(-1, -1, -1)
 , m_markerCount(0)
{
}

void PatternSolver::LoadBaseImage(QString filePath)
{
    qDebug() << filePath;
    m_baseImage = QImage(filePath);

    aruco::MarkerDetector detector;
    cv::Mat cvBaseImage = cv::imread(filePath.toStdString());

    vector<aruco::Marker> markers = detector.detect(cvBaseImage);
    if (markers.size() == 0)
    {
        qDebug() << "Detection failed on base image!";
        return;
    }

    vector<int> unprocessedMarkers;

    for(unsigned int idx = 0; idx < markers.size(); ++idx)
    {
        DataPoint currentMarker;
        currentMarker.X = static_cast<int>(markers[idx].getCenter().x);
        currentMarker.Y = static_cast<int>(markers[idx].getCenter().y);
        currentMarker.Z = static_cast<int>(markers[idx].getArea());

        if (markers[idx].id == ANCHOR_POINT_ID)
        {
            m_anchorAbs = currentMarker;
            m_anchorPercent = sizeToPercent(m_anchorAbs, CAPTURE_IMG_WIDTH, CAPTURE_IMG_HEIGHT);
            qDebug() << "ANCHOR coords: (" << m_anchorAbs.X << "," << m_anchorAbs.Y << ")";
            qDebug() << "ANCHOR % coords: (" << m_anchorPercent.X << "," << m_anchorPercent.Y << ")";
            continue;
        }

        if (m_anchorAbs.X == -1)
        {
            unprocessedMarkers.push_back(markers[idx].id);
            m_structure.insert(markers[idx].id, currentMarker);
            continue;
        }

        UpdateRelativeCoordinates(currentMarker, m_anchorAbs);
        currentMarker = sizeToPercent(currentMarker, CAPTURE_IMG_WIDTH, CAPTURE_IMG_HEIGHT);

        m_structure.insert(markers[idx].id, currentMarker);
        qDebug() << idx << ": " << "(" << currentMarker.X << "," << currentMarker.Y << "," << currentMarker.Z << ")";
    }

    for (auto it = unprocessedMarkers.begin(); it != unprocessedMarkers.end(); ++it)
    {
        int id = (*it);
        //qDebug() << "Unprocessed marker " << id << ":";
        auto markerIt = m_structure.find(id);
        //qDebug() << "Unprocessed Point are: (" << markerIt.value().X << "," << markerIt.value().Y << ")";
        UpdateRelativeCoordinates(markerIt.value(), m_anchorAbs);
        //qDebug() << "Relativized Point are: (" << markerIt.value().X << "," << markerIt.value().Y << ")";
        markerIt.value() = sizeToPercent(markerIt.value(), CAPTURE_IMG_WIDTH, CAPTURE_IMG_HEIGHT);
        //qDebug() << "Rescaled Point are: (" << markerIt.value().X << "," << markerIt.value().Y << ")";
    }

    m_markerCount = static_cast<int>(markers.size()) - 1;
    qDebug() << "Total markers: " << m_markerCount;
}

DataPoint PatternSolver::rescaleForTerminal(const DataPoint& original)
{
    DataPoint rescaled = original;
    rescaled.Z = 0;

    DataPoint frameSize(m_frameSize.width, m_frameSize.height, 0);

    rescaled = sizeToPercent(original, m_frameSize.width, m_frameSize.height);
    rescaled = percentToSize(rescaled, TERMINAL_WIDTH, TERMINAL_HEIGHT);
    //qDebug() << "Upscaled coords are: (" << targetDestination.X << "," << targetDestination.Y << ")";

    return rescaled;
}

void PatternSolver::drawTargetForMarker(int markerId, cv::Mat frame)
{
     auto it = m_structure.constFind(markerId);
     if (it == m_structure.end())
     {
         return;
     }

     DataPoint imageSize(frame.size().width, frame.size().height, 0);

     DataPoint targetDestination = it.value();

     targetDestination = getAbsoluteCoordinates(targetDestination);
     //ScaleAsPercentOf(targetDestination, CAPTURE_REF);
     //qDebug() << "Unprocessed coords are: (" << targetDestination.X << "," << targetDestination.Y << ")";
     //qDebug() << "Absolute coords are: (" << targetDestination.X << "," << targetDestination.Y << ")";
     targetDestination = percentToSize(targetDestination, imageSize.X, imageSize.Y);
     //qDebug() << "Upscaled coords are: (" << targetDestination.X << "," << targetDestination.Y << ")";
     DrawTarget(frame, targetDestination, 20);
}

DataPoint PatternSolver::getTargetForMarker(int markerId, int frameW, int frameH)
{
    auto it = m_structure.constFind(markerId);
    if (it == m_structure.end())
    {
        return DataPoint(-1, -1, -1);
    }

    DataPoint imageSize(frameW, frameH, 0);

    DataPoint targetDestination = it.value();
    //ScaleAsPercentOf(targetDestination, CAPTURE_REF);
    //qDebug() << "Unprocessed coords are: (" << targetDestination.X << "," << targetDestination.Y << ")";
    targetDestination = getAbsoluteCoordinates(targetDestination);
    targetDestination = percentToSize(targetDestination, imageSize.X, imageSize.Y);
    //qDebug() << "Upscaled coords are: (" << targetDestination.X << "," << targetDestination.Y << ")";

    return targetDestination;
}

void PatternSolver::relativizeMarkerCoordinates()
{}

DataPoint PatternSolver::getAbsoluteCoordinates(DataPoint const input)
{
    DataPoint output = input;
    DataPoint offset(-m_anchorPercent.X,
                 m_anchorPercent.Y,
                 0);

    UpdateRelativeCoordinates(output, offset);

    return output;
}

QImage PatternSolver::processFrame(const QString &path)
{
    cv::Mat frame = cv::imread(path.toStdString());
    QList<DataPoint> unused;
    cv::Mat processed = processCvMatFrame(frame, unused, false);

    if (processed.empty())
    {
        return QImage(path);
    }

    QImage output(frame.data,
                  frame.cols,
                  frame.rows,
                  static_cast<int>(frame.step),
                  QImage::Format_RGB888);
    return output.rgbSwapped();
}

QImage PatternSolver::processFrame(cv::Mat frame)
{
    QList<DataPoint> unused;
    cv::Mat processed = processCvMatFrame(frame, unused, false);

    if (processed.empty())
    {
        return QImage();
    }

    QImage output(frame.data,
                  frame.cols,
                  frame.rows,
                  static_cast<int>(frame.step),
                  QImage::Format_RGB888);
    return output.rgbSwapped();
}

QImage PatternSolver::processFrame(cv::Mat frame, QList<DataPoint> &markerPoints)
{
    cv::Mat processed = processCvMatFrame(frame, markerPoints, true);

    if (processed.empty())
    {
        return QImage();
    }

    QImage output(frame.data,
                  frame.cols,
                  frame.rows,
                  static_cast<int>(frame.step),
                  QImage::Format_RGB888);
    return output.rgbSwapped();
}

QList<DataPoint> PatternSolver::processFrameFromCvMat(cv::Mat frame)
{
    QList<DataPoint> markerPoint;
    processCvMatFrame(frame, markerPoint);

    return markerPoint;
}

cv::Mat PatternSolver::processCvMatFrame(cv::Mat frame, QList<DataPoint> &PointsList, bool outputMarkerList)
{
    // get frameSize before anything else
    m_frameSize = frame.size();
    if (m_markerCount == 0)
    {
        qDebug() << "[SUCCESS] All objects matched!";
        return frame;
    }

    aruco::MarkerDetector MDetector;
    vector<aruco::Marker> markers = MDetector.detect(frame);

    if (markers.size() == 0)
    {
        return frame;
    }

    for(unsigned int idx = 0; idx < markers.size(); ++idx)
    {
        DataPoint markerPoint = DataPoint(static_cast<int>(markers[idx].getCenter().x),
                                  static_cast<int>(markers[idx].getCenter().y),
                                  0);

        // Update anchor point location - allows us to reposition the entire structure
        // by changing anchor marker location in space
        if (markers[idx].id == ANCHOR_POINT_ID)
        {
            markerPoint = sizeToPercent(markerPoint, m_frameSize.width, m_frameSize.height);
            m_structure[ANCHOR_POINT_ID] = markerPoint;
            m_anchorPercent = markerPoint;
            continue;
        }

        // we already solved this one, skip
        if (m_matched.contains(markers[idx].id))
        {
            continue;
        }

        DrawMarker(frame, markerPoint, 20);
        drawTargetForMarker(markers[idx].id, frame);
        markers[idx].draw(frame, cv::Scalar(0,0,255), 2,true,true);

        DataPoint targetPoint = getTargetForMarker(markers[idx].id, frame.size().width, frame.size().height);

        //qDebug() << "[TRACKING] Marker: (" << markerPoint.X << ", " << markerPoint.Y << " ," << markerPoint.Z << ") Target: " << targetPoint.X << ", " << targetPoint.Y << " ," << targetPoint.Z;

        if (targetPoint == markerPoint)
        {
            m_matched.append(markers[idx].id);
            qDebug() << "[MATCH] Marker ID: " << markers[idx].id << " with TARGET";
            m_markerCount--;
        }


        if (!outputMarkerList)
        {
            continue;
        }

        targetPoint.type = TARGET;
        targetPoint.Z = 0;
        markerPoint.type = MARKER;
        markerPoint.Z = 0;

        targetPoint = rescaleForTerminal(targetPoint);
        markerPoint = rescaleForTerminal(markerPoint);

        PointsList.append(targetPoint);
        PointsList.append(markerPoint);

        //qDebug() << "Area: " << markers[idx].getArea();
        //qDebug() << " Coordinates: " << markers[idx].getCenter().x << " " << markers[idx].getCenter().y;
        //qDebug() << " Id: " << markers[idx].id << '\n';
    }

    return frame;
}
