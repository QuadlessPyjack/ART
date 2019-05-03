#ifndef PATTERNSOLVER_H
#define PATTERNSOLVER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QImage>

#include "utils.h"

// forward declarations
class QImage;

class PatternSolver : public QObject
{
    Q_OBJECT
public:
    explicit PatternSolver(QObject *parent = nullptr);
    Q_INVOKABLE void LoadBaseImage(QString filePath);
    QImage processFrame(const QString &path);
    QImage processFrame(cv::Mat frame);
    QImage processFrame(cv::Mat frame, QList<DataPoint> &markerPoints);
    QList<DataPoint> processFrameFromCvMat(cv::Mat frame);

private:
    cv::Mat processCvMatFrame(cv::Mat frame, QList<DataPoint> &PointsList, bool outputMarkerList = true);
    void drawTargetForMarker(int markerId, cv::Mat frame);
    DataPoint getTargetForMarker(int markerId, int frameW = 0, int frameH = 0);
    DataPoint rescaleForTerminal(const DataPoint &original);
    void relativizeMarkerCoordinates();
    DataPoint getAbsoluteCoordinates(const DataPoint input);
    QMap<int,DataPoint> m_structure;
    QList<int> m_matched;
    DataPoint m_anchorAbs;
    DataPoint m_anchorPercent;
    QImage m_baseImage;
    int m_markerCount;
    cv::Size m_frameSize;
};

#endif // PATTERNSOLVER_H
