#ifndef SPHERE_H
#define SPHERE_H
#include "Point.h"
#include "Color.h"
#include "Ray.h"
#include "Object.h"
#include <math.h>
#include <cfloat>

using namespace std;

class Sphere: public Object{
    private:
        Point center;
        double radius;
    public:
        Sphere(Point, double, Color, float, float, float, float, float);
        double intersect(Ray*);
        Point getNormal(Point point);
};

#endif
