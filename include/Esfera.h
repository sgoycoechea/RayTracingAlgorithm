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
        Color* color;
        double rad;

    public:
        Esfera(Point*, double, Color*);
        Point* getCentro();
        double getRad();
        Color* getColor();
        double intersectar(Rayo*);
        Point* getNormal(Point* punto);
};

#endif
