#ifndef OBJECT_H
#define OBJECT_H

#include "Point.h"
#include "Color.h"
#include "Ray.h"

using namespace std;

class Object {
    private:
        Color color;
        float ambientCoefficient;
        float transmissionCoefficient;
        float specularCoefficient;
        float diffusionCoefficient;
        float refractiveIndex;

    public:
      Object(Color,float, float, float, float, float);
      virtual double intersect(Ray*) = 0;
      virtual Point getNormal(Point point) = 0;
      virtual Color getColor();
      virtual float getTransmissionCoefficient();
      virtual float getSpecularCoefficient();
      virtual float getDiffusionCoefficient();
      virtual float getAmbientCoefficient();
      virtual float getRefractiveIndex();
};

#endif
