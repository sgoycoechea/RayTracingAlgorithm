#ifndef TRIANGLE_H
#define TRIANGLE_H
#include "Point.h"
#include "Color.h"
#include "Ray.h"
#include "Object.h"
#include <math.h>
#include <cfloat>

using namespace std;

class Triangle: public Object{
    private:
        Point point1;
        Point point2;
        Point point3;
        float triangleArea(Point, Point, Point);
        bool isPointInTriangle(Point, Point, Point, Point);
    public:
        Triangle(Point, Point, Point, Color, float, float, float, float, float);
        double intersect(Ray*);
        Point getNormal(Point point);
};

#endif
