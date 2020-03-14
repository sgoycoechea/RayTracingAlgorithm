#include "../include/Triangle.h"
#include <iostream>
#include <fstream>

using namespace std;

Triangle::Triangle(Point point1, Point point2, Point point3, Color color, float transmissionCoefficient, float specularCoefficient, float diffusionCoefficient, float ambientCoefficient, float refractiveIndex):Object(color, transmissionCoefficient, specularCoefficient, diffusionCoefficient, ambientCoefficient, refractiveIndex){
    this->point1 = point1;
    this->point2 = point2;
    this->point3 = point3;
}

Point Triangle::getNormal(Point point){
    Point vector12 = point1 - point2;
    Point vector13 = point1 - point3;
    Point normal = vector12.crossProduct(vector13);
    normal.normalize();

    return normal;
}

double Triangle::intersect(Ray* ray) {
    Point normal = getNormal(point1);
    double dotProduct = normal.dotProduct(ray->getDirection());

    if (dotProduct < 0.0001 && dotProduct > 0.0001) {
        return FLT_MAX; // No intersection, the plain that contains the triangle is parallel to the ray
    }

    double ret = normal.dotProduct(point1 - ray->getOrigin()) / dotProduct;
    if (ret < 0.001) {
        return FLT_MAX; // The plain that contains the triangle is behind the camera
    }

    Point intersection = ray->getOrigin() + (ray->getDirection() * ret);

    // Check that the intersection of the ray with the plain is inside the triangle
    if (isPointInTriangle(intersection, point1, point2, point3))
        return ret;

    return FLT_MAX;
};

float Triangle::triangleArea(Point p1, Point p2, Point p3)
{
    Point normal = getNormal(point1);
    Point vector13 = p1 - p3;
    Point vector23 = p2 - p3;
    Point crossProduct = vector13.crossProduct(vector23);

    float res = sqrt(crossProduct.dotProduct(crossProduct)) / 2;
    if (crossProduct.dotProduct(normal) < 0)
        res *= -1;

    return res;
}

// Source:   https://math.stackexchange.com/questions/4322/check-whether-a-point-is-within-a-3d-triangle
bool Triangle::isPointInTriangle(Point pt, Point v1, Point v2, Point v3)
{
    float areaABC = triangleArea(v1, v2, v3);
    float areaPBC = triangleArea(pt, v2, v3);
    float areaPCA = triangleArea(pt, v3, v1);

    float alpha = areaPBC / areaABC;
    float beta = areaPCA / areaABC;
    float gamma = 1 - alpha - beta;

    return alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1 && gamma >= 0 && gamma <= 1;
}

