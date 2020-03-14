#ifndef OBJETO_H
#define OBJETO_H

#include "Point.h"
#include "Color.h"
#include "Ray.h"

using namespace std;

class Objeto {
    private:
        Color color;
        float coefAmbiente;
        float coefTransmision;
        float coefEspecular;
        float coefDifuso;
        float indiceRefraccion;

    public:
      Objeto(Color,float, float, float, float, float);
      virtual double intersectar(Rayo*) = 0;
      virtual Point getNormal(Point punto) = 0;
      virtual Color getColor();
      virtual float getCoefTransmision();
      virtual float getCoefEspecular();
      virtual float getCoefDifuso();
      virtual float getCoefAmbiente();
      virtual float getIndiceRefraccion();
};

#endif
