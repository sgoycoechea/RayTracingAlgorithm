#ifndef ESFERA_H
#define ESFERA_H
#include "Point.h"
#include "Color.h"
#include "Rayo.h"
#include <math.h>
#include <cfloat>

#define M_PI 3.1415926

using namespace std;

class Esfera{
    private:
        Point* centro;
        Color* color;
        double rad;

    public:
        Esfera(Point*, double, Color*);
        Point* getCentro();
        Color* getColor();
        double getRad();
        double intersectar(Rayo*);
        Point* getNormal(Point* punto);
};

#endif
