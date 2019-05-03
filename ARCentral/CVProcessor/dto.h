#ifndef DTO_H
#define DTO_H

struct DataPoint
{
    DataPoint() {}
    DataPoint(int x, int y, int z, bool isRelative = false)
        : X(x), Y(y), Z(z), IsRelative(isRelative) {}
    int X;
    int Y;
    int Z;
    int type;
    bool IsRelative = false;
    bool IsVisible = true;

    bool operator== (const DataPoint &rhs)
    {
        if (this->X != rhs.X) return false;
        if (this->Y != rhs.Y) return false;
        //if (this->Z != rhs.Z) return false; currently Z = depth UNUSED
        if (this->IsRelative != rhs.IsRelative) return false;

        return true;
    }
};

struct Coords
{
    int X;
    int Y;
    int Z;
    int type;
};

enum CoordType
{
    TARGET = 0,
    MARKER = 1,
};

#endif // DTO_H
