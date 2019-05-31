#include "../include/Point.h"

using namespace std;

Point::Point(float x, float y, float z){
    this->x = x;
    this->y = y;
    this->z = z;
}

float Point::getX() {
  return x;
}

float Point::getY() {
  return y;
}

float Point::getZ() {
  return z;
}

double Point::magnitude(){
    return sqrt(pow(x,2) + pow(y,2) + pow(z,2));
}

void Point::normalizar(){
    double mag = magnitude();
    x = x / mag;
    y = y / mag;
    z = z / mag;
}

float Point::dotProduct(Point* vector2){
    return (x * vector2->getX()) + (y * vector2->getY()) + (z * vector2->getZ());
}

Point* Point::operator-(Point* vector2){
    Point* res = new Point(x - vector2->getX(), y - vector2->getY(), z - vector2->getZ());
    return res;
}

Point* Point::operator+(Point* vector2){
    Point* res = new Point(x + vector2->getX(), y + vector2->getY(), z + vector2->getZ());
    return res;
}

Point* Point::operator/(double divisor){
    Point* res = new Point(x / divisor, y / divisor, z / divisor);
    return res;
}

Point* Point::operator*(double divisor){
    Point* res = new Point(x * divisor, y * divisor, z * divisor);
    return res;
}
