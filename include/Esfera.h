#ifndef ESFERA_H
#define ESFERA_H
#include "Point.h"
#include "Color.h"
#include "Rayo.h"
#include "Objeto.h"
#include <math.h>
#include <cfloat>

#define M_PI 3.1415926

using namespace std;

class Esfera: public Objeto{
    private:
        Point* centro;
        double rad;
    public:
        Esfera(Point*, double, Color*, float, float, float);
        double intersectar(Rayo*);
        Point* getNormal(Point* punto);
};

#endif
