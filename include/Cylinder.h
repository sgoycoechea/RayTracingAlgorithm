#ifndef CYLINDER_H
#define CYLINDER_H
#include "Point.h"
#include "Color.h"
#include "Ray.h"
#include "Object.h"
#include <math.h>
#include <cfloat>

using namespace std;

class Cylinder: public Object{
    private:
        Point baseCenter;
        Point direction;
        double radius;
        double height;
        double intersectBody(Ray*);
        double intersectBase(Ray*, Point);
    public:
        Cylinder(Point, Point, double, double, Color, float, float, float, float, float);
        double intersect(Ray*);
        Point getNormal(Point point);
};

#endif
