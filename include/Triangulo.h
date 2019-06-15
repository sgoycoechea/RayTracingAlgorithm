#ifndef TRIANGULO_H
#define TRIANGULO_H
#include "Point.h"
#include "Color.h"
#include "Rayo.h"
#include "Objeto.h"
#include <math.h>
#include <cfloat>

#define M_PI 3.1415926

using namespace std;

class Triangulo: public Objeto{
    private:
        Point punto1;
        Point punto2;
        Point punto3;
        float areaTriangulo(Point, Point, Point);
        bool puntoEnTriangulo(Point, Point, Point, Point);
    public:
        Triangulo(Point, Point, Point, Color, float, float, float, float);
        double intersectar(Rayo*);
        Point getNormal(Point punto);
};

#endif
