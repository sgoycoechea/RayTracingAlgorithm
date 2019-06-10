#include "../include/Triangulo.h"
#include <iostream>
#include <fstream>

using namespace std;

Triangulo::Triangulo(Point* punto1, Point* punto2, Point* punto3, Color* color, float coefTransmision, float coefEspecular, float coefDifuso, float indiceRefraccion):Objeto(color, coefTransmision, coefEspecular, coefDifuso, indiceRefraccion){
    this->punto1 = punto1;
    this->punto2 = punto2;
    this->punto3 = punto3;
}

Point* Triangulo::getNormal(Point* punto){
    Point* vector12 = (*punto1) - punto2;
    Point* vector13 = (*punto1) - punto3;
    Point* normal = vector12->productoVectorial(vector13);
    normal->normalizar();

    return normal;
}

double Triangulo::intersectar(Rayo* rayo) {
    Point* normal = getNormal(punto1);
    double prodInterno = normal->dotProduct(rayo->getDireccion());

    if (prodInterno < 0.0001 && prodInterno > 0.0001) {
        return FLT_MAX; // No hay interseccion, el plano que contiene al triangulo y el rayo son paralelos
    }

    double ret = normal->dotProduct((*punto1) - rayo->getOrigen()) / prodInterno;
    if (ret < 0.001) {
        return FLT_MAX; // El plano que contiene al triangulo esta atras de la camara
    }

    Point* interseccion = (*rayo->getOrigen()) + ((*rayo->getDireccion()) * ret);

    // Chequear que la interseccion este dentro del triangulo
    if (puntoEnTriangulo(interseccion, punto1, punto2, punto3))
        return ret;

    return FLT_MAX;
};

float Triangulo::areaTriangulo(Point* p1, Point* p2, Point* p3)
{
    Point* normal = getNormal(punto1);
    Point* vector13 = (*p1) - p3;
    Point* vector23 = (*p2) - p3;
    Point* prodVectorial = vector13->productoVectorial(vector23);

    float res = sqrt(prodVectorial->dotProduct(prodVectorial)) / 2;
    if (prodVectorial->dotProduct(normal) < 0)
        res *= -1;

    return res;
}

// Fuente:   https://math.stackexchange.com/questions/4322/check-whether-a-point-is-within-a-3d-triangle
bool Triangulo::puntoEnTriangulo(Point* pt, Point* v1, Point* v2, Point* v3)
{
    float areaABC = areaTriangulo(v1, v2, v3);
    float areaPBC = areaTriangulo(pt, v2, v3);
    float areaPCA = areaTriangulo(pt, v3, v1);

    float alfa = areaPBC / areaABC;
    float beta = areaPCA / areaABC;
    float gama = 1 - alfa - beta;

    return alfa >= 0 && alfa <= 1 && beta >= 0 && beta <= 1 && gama >= 0 && gama <= 1;
}

