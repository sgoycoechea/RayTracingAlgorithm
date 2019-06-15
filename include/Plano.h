#ifndef PLANO_H
#define PLANO_H
#include "Point.h"
#include "Color.h"
#include "Rayo.h"
#include "Objeto.h"
#include <math.h>
#include <cfloat>

#define M_PI 3.1415926

using namespace std;

class Plano: public Objeto{
    private:
        Point punto;
        Point normal;
    public:
        Plano(Point, Point, Color,float, float, float, float);
        double intersectar(Rayo*);
        Point getNormal(Point punto);
};

#endif
