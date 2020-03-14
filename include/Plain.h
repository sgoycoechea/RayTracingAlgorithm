#ifndef PLANO_H
#define PLANO_H
#include "Point.h"
#include "Color.h"
#include "Ray.h"
#include "Object.h"
#include <math.h>
#include <cfloat>

using namespace std;

class Plano: public Objeto{
    private:
        Point punto;
        Point normal;
    public:
        Plano(Point, Point, Color,float, float, float, float, float);
        double intersectar(Rayo*);
        Point getNormal(Point punto);
};

#endif
