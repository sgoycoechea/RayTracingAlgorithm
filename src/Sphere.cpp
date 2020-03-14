#include "../include/Sphere.h"
#include <iostream>
#include <fstream>

using namespace std;

Sphere::Sphere(Point center, double radius, Color color, float transmissionCoefficient, float specularCoefficient, float diffusionCoefficient, float ambientCoefficient, float refractiveIndex):Object(color, transmissionCoefficient, specularCoefficient, diffusionCoefficient, ambientCoefficient, refractiveIndex){
    this->center = center;
    this->radius = radius;
}

Point Sphere::getNormal(Point point){
    return center - (point);
}

double Sphere::intersect(Ray* ray) {
    double directionX = ray->getDirection().getX();
    double directionY = ray->getDirection().getY();
    double directionZ = ray->getDirection().getZ();
    double originX = ray->getOrigin().getX();
    double originY = ray->getOrigin().getY();
    double originZ = ray->getOrigin().getZ();

    //Bhaskaras
    double a = pow(directionX,2) + pow(directionY,2) + pow(directionZ,2);
    double b = 2 * ( directionX * (originX - center.getX()) + directionY * (originY - center.getY()) + directionZ * (originZ - center.getZ()) );
    double c = pow(originX - center.getX(), 2) + pow(originY - center.getY(), 2) + pow(originZ - center.getZ(), 2) - pow(radius, 2);
    double delta = b*b - 4 * a * c;


    if (delta < 0){
        return FLT_MAX; // No intersection
    }

    double root1 = (-b - sqrt(delta)) / (2 * a);
    double root2 = (-b + sqrt(delta)) / (2 * a);

    if (root2 < 0.01)
        return FLT_MAX; // The sphere is behind the camera

    if (root1 > 0.01)
        return root1; // The sphere is in front of the camera
    else
        return root2; // The camera is inside the sphere
};


