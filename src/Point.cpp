#include "../include/Point.h"

using namespace std;

Point::Point(float x, float y, float z){
    this->x = x;
    this->y = y;
    this->z = z;
}

Point::Point(){
    this->x = 0;
    this->y = 0;
    this->z = 0;
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

void Point::normalize(){
    double mag = magnitude();
    x = x / mag;
    y = y / mag;
    z = z / mag;
}

Point Point::crossProduct(Point vector2){
    double resX = y * vector2.getZ() - z * vector2.getY();
    double resY = - (x * vector2.getZ() - z * vector2.getX());
    double resZ = x * vector2.getY() - y * vector2.getX();

    return Point(resX,resY,resZ);
}

float Point::dotProduct(Point vector2){
    return (x * vector2.getX()) + (y * vector2.getY()) + (z * vector2.getZ());
}

Point Point::operator-(Point vector2){
    Point res = Point(x - vector2.getX(), y - vector2.getY(), z - vector2.getZ());
    return res;
}

Point Point::operator+(Point vector2){
    Point res = Point(x + vector2.getX(), y + vector2.getY(), z + vector2.getZ());
    return res;
}

Point Point::operator/(double divisor){
    Point res = Point(x / divisor, y / divisor, z / divisor);
    return res;
}

Point Point::operator*(double divisor){
    Point res = Point(x * divisor, y * divisor, z * divisor);
    return res;
}
