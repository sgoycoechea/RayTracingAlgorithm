#ifndef CILINDRO_H
#define CILINDRO_H
#include "Point.h"
#include "Color.h"
#include "Ray.h"
#include "Object.h"
#include <math.h>
#include <cfloat>

using namespace std;

class Cilindro: public Objeto{
    private:
        Point centroBase;
        Point direccion;
        double rad;
        double altura;
        double intersectarCuerpo(Rayo*);
        double intersectarTapa(Rayo*, Point);
    public:
        Cilindro(Point, Point, double, double, Color, float, float, float, float, float);
        double intersectar(Rayo*);
        Point getNormal(Point punto);
};

#endif
