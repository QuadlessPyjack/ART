#ifndef UTILS_H
#define UTILS_H

#include <QDebug>
#include <opencv2/imgproc.hpp>

#include <dto.h>

static const unsigned short ANCHOR_POINT_ID = 12;
static const long CAPTURE_IMG_WIDTH = 3840;
static const long CAPTURE_IMG_HEIGHT = 2160;
static const int TERMINAL_WIDTH = 128;
static const int TERMINAL_HEIGHT = 64;

typedef unsigned int uint;

static const DataPoint CAPTURE_REF(CAPTURE_IMG_WIDTH, CAPTURE_IMG_HEIGHT, 0);
static const DataPoint TERMINAL_REF(TERMINAL_WIDTH, TERMINAL_HEIGHT, 0);

// it is implied that original is a percentage-scaled value
inline DataPoint sizeToPercent(const DataPoint& original, const float refW, const float refH, const uint scale = 100)
{
    int x = static_cast<int>((original.X / refW) * scale);
    int y = static_cast<int>((original.Y / refH) * scale);
    int z = static_cast<int>(original.Z / (refW) * (original.Z / refH) * scale);

    DataPoint scaled(x, y, z);
    scaled.type = original.type;
    scaled.IsVisible = original.IsVisible;
    scaled.IsRelative = original.IsRelative;

    return scaled;
}

// requires that the initial percentage scale factor is known - ie. 10^x x E (-inf;+inf)
inline DataPoint percentToSize(const DataPoint& original, const int refW, const int refH, const float scale = 100)
{
    if (scale == 0.0f)
    {
        return original;
    }
    int x = static_cast<int>((original.X / scale) * refW);
    int y = static_cast<int>((original.Y / scale) * refH);
    int z = static_cast<int>(original.Z / scale * x*y);

    DataPoint scaled(x, y, z);
    scaled.type = original.type;
    scaled.IsVisible = original.IsVisible;
    scaled.IsRelative = original.IsRelative;

    return scaled;
}

inline void DrawTarget(cv::Mat &image, const DataPoint centre, const int size = 4)
{
    qDebug() << "Target Point: (" << centre.X << "," << centre.Y << ")";
    //    A
    //    |
    // C-----D
    //    |
    //    B
    int thickness = 4;
    int lineType = 8;

    int Cx = static_cast<int>(centre.X);
    int Cy = static_cast<int>(centre.Y);

    //AB
    line(image,
         cv::Point(Cx - size, Cy),
         cv::Point(Cx + size, Cy),
         cv::Scalar(255,0,0), thickness, lineType);

    //CD
    line(image,
         cv::Point(Cx, Cy - size),
         cv::Point(Cx, Cy + size),
         cv::Scalar(255,0,0), thickness, lineType);
}

inline void DrawMarker(cv::Mat image, DataPoint centre, const int size = 4)
{
    int thickness = 4;
    int lineType = 8;

    // A ------ B
    //  |      |
    //  |______|
    // D        C
    // A = (Cx - size, Cy - size)
    // B = (Cx + size, Cy - size)
    // C = (Cx + size, Cy + size)
    // D = (Cx - size, Cy + size)

    int Cx = static_cast<int>(centre.X);
    int Cy = static_cast<int>(centre.Y);

    // AB
    line(image,
         cv::Point(Cx - size, Cy - size),
         cv::Point(Cx + size, Cy - size),
         cv::Scalar(255,0,0), thickness, lineType);
    //BC
    line(image,
         cv::Point(Cx + size, Cy - size),
         cv::Point(Cx + size, Cy + size),
         cv::Scalar(255,0,0), thickness, lineType);
    //CD
    line(image,
         cv::Point(Cx + size, Cy + size),
         cv::Point(Cx - size, Cy + size),
         cv::Scalar(255,0,0), thickness, lineType);
    //DA
    line(image,
         cv::Point(Cx - size, Cy + size),
         cv::Point(Cx - size, Cy - size),
         cv::Scalar(255,0,0), thickness, lineType);
}

inline void ScaleAsPercentOf(DataPoint &input, DataPoint imageSize)
{
    //qDebug() << "Frame size: (" << imageSize.X << "," << imageSize.Y << ")";
    input.X = static_cast<int>(input.X / imageSize.X * 100);
    input.Y = static_cast<int>(input.Y / imageSize.Y * 100);

    //input.X = static_cast<int>(imageSize.X * (input.X * 0.01f));
    //input.Y = static_cast<int>(imageSize.Y * (input.Y * 0.01f));
}

inline void UpdateRelativeCoordinates(DataPoint &input, DataPoint const &referencePoint)
{
    input.X = input.X - referencePoint.X;
    input.Y = -(input.Y - referencePoint.Y);
    //input.X = input.X + referencePoint.X;
    //input.Y = input.Y + referencePoint.Y;

    input.IsRelative = true;
}

#endif // UTILS_H
