#include "../include/Plain.h"
#include <iostream>
#include <fstream>

using namespace std;

Plain::Plain(Point point, Point normal, Color color, float transmissionCoefficient, float specularCoefficient, float diffusionCoefficient, float ambientCoefficient, float refractiveIndex):Object(color, transmissionCoefficient, specularCoefficient, diffusionCoefficient, ambientCoefficient, refractiveIndex){
    this->point = point;
    normal.normalize();
    this->normal = normal;
}

Point Plain::getNormal(Point point){
    return normal * -1;
}

double Plain::intersect(Ray* ray) {
    double dotProduct = normal.dotProduct(ray->getDirection());

    if (dotProduct < 0.0001 && dotProduct > 0.0001) {
        // No intersection, plain and ray are parallel
        return FLT_MAX;
    }

    double ret = normal.dotProduct(point - ray->getOrigin()) / dotProduct;
    if (ret < 0.001) {
        // The plain is behind the camera
        return FLT_MAX;
    }

    return ret;
};


