#include "../include/Cylinder.h"
#include <iostream>
#include <fstream>

using namespace std;

Cylinder::Cylinder(Point baseCenter, Point direction, double radius, double height, Color color, float transmissionCoefficient, float specularCoefficient, float diffusionCoefficient, float ambientCoefficient, float refractiveIndex):Object(color, transmissionCoefficient, specularCoefficient, diffusionCoefficient, ambientCoefficient, refractiveIndex){
    this->baseCenter = baseCenter;
    direction.normalize();
    this->direction = direction;
    this->height = height;
    this->radius = radius;
}

Point Cylinder::getNormal(Point point){

    // Check if point is in one of the bases
    Point baseCenter2 = baseCenter + (direction * height);
    Point vec1 = point - baseCenter;
    Point vec2 = point - baseCenter2;

    if (vec1.dotProduct(direction) < 0.001 && vec1.dotProduct(direction) > -0.001){
        return direction;
    }

    if (vec2.dotProduct(direction) < 0.001 && vec2.dotProduct(direction) > -0.001){
        return direction * -1;
    }


    Point centerPoint =  baseCenter - point;
    // Subtract height of the point within the cylinder
    Point ret = centerPoint - direction * centerPoint.dotProduct(direction);

    ret.normalize();
    return ret;
}

double Cylinder::intersectBody(Ray* ray){
    Point originCenterDifference = ray->getOrigin() - baseCenter;
    double directionesDot = ray->getDirection().dotProduct(direction);

    //Bhaskaras
    double a = 1 - directionesDot * directionesDot;
    double b = 2 * (originCenterDifference.dotProduct(ray->getDirection()) - directionesDot * originCenterDifference.dotProduct(direction));
    double c = originCenterDifference.dotProduct(originCenterDifference) - originCenterDifference.dotProduct(direction) * originCenterDifference.dotProduct(direction) - radius * radius;
    double delta = b*b - 4 * a * c;

    if (delta < 0){
        return FLT_MAX; // No intersection
    }

    double root1 = (-b - sqrt(delta)) / (2 * a);
    double root2 = (-b + sqrt(delta)) / (2 * a);

    if (root2 < 0.01) {
        return FLT_MAX; // The cylinder is completely behind the camera
    }

    // There is an intersection with the infinite cylinder, we still need to check if it's within its height

    double centerProjection = baseCenter.dotProduct(direction);

    Point intersection1 = ray->getOrigin() + (ray->getDirection() * root1);
    double intersectionProjection1 = intersection1.dotProduct(direction);
    if (root1 >= 0.01 && intersectionProjection1 > centerProjection && intersectionProjection1 < centerProjection + height) {
        return root1;
    }

    Point intersection2 = ray->getOrigin() + (ray->getDirection() * root2);
    double intersectionProjection2 = intersection2.dotProduct(direction);
    if (root2 >= 0.01 && intersectionProjection2 > centerProjection && intersectionProjection2 < centerProjection + height) {
        return root2;
    }

    return FLT_MAX; // No intersection

}

double Cylinder::intersectBase(Ray* ray, Point baseCenter){
    double dotProduct = direction.dotProduct(ray->getDirection());

    if (dotProduct == 0) {
        return FLT_MAX; // No intersection, the base plain and the ray are parallel
    }

    double ret = direction.dotProduct(baseCenter - ray->getOrigin()) / dotProduct;

    if (ret < 0.01) { // The plain is behind the ray
        return FLT_MAX;
    }

    Point intersection = ray->getOrigin() + (ray->getDirection() * ret);

    Point centerIntersection = intersection - baseCenter;
    double centerDistance = sqrt(centerIntersection.dotProduct(centerIntersection));


    if (centerDistance < radius)
        return ret;

    // It intersects the plain of the base but not the base itself
    return FLT_MAX;
}

double Cylinder::intersect(Ray* ray) {

    Point baseCenter2 = baseCenter + (direction * height);

    double body = intersectBody(ray);
    double base1 = intersectBase(ray, baseCenter);
    double base2 = intersectBase(ray, baseCenter2) ;
    double closestBase = base1 < base2 ? base1 : base2;

    return body < closestBase ? body : closestBase;
};
