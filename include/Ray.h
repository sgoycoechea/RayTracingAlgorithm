#ifndef RAY_H
#define RAY_H
#include "Point.h"

using namespace std;

class Ray{
    private:
        Point direction;
        Point origen;

    public:
        Ray(Point, Point);
        Point getDirection();
        Point getOrigin();
};

#endif
