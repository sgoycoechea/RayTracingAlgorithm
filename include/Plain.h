#ifndef PLAIN_H
#define PLAIN_H
#include "Point.h"
#include "Color.h"
#include "Ray.h"
#include "Object.h"
#include <math.h>
#include <cfloat>

using namespace std;

class Plain: public Object{
    private:
        Point point;
        Point normal;
    public:
        Plain(Point, Point, Color,float, float, float, float, float);
        double intersect(Ray*);
        Point getNormal(Point point);
};

#endif
