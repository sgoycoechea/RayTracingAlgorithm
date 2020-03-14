#ifndef ESFERA_H
#define ESFERA_H
#include "Point.h"
#include "Color.h"
#include "Ray.h"
#include "Object.h"
#include <math.h>
#include <cfloat>

using namespace std;

class Esfera: public Objeto{
    private:
        Point centro;
        double rad;
    public:
        Esfera(Point, double, Color, float, float, float, float, float);
        double intersectar(Rayo*);
        Point getNormal(Point punto);
};

#endif
