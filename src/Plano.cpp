#include "../include/Plano.h"
#include <iostream>
#include <fstream>

using namespace std;

Plano::Plano(Point* punto, Point* normal, Color* color, float opacidadR, float opacidadG, float opacidadB):Objeto(opacidadR, opacidadG, opacidadB, color){
    this->punto = punto;
    normal->normalizar();
    this->normal = normal;

}

Point* Plano::getNormal(Point* punto){
    return (*normal) * -1;
}

double Plano::intersectar(Rayo* rayo) {
    double prodInterno = normal->dotProduct(rayo->getDireccion());

    if (prodInterno < 0.0001 && prodInterno > 0.0001) {
        // No hay interseccion, plano y rayo son paralelos
        return FLT_MAX;
    }

    double ret = normal->dotProduct((*punto) - rayo->getOrigen()) / prodInterno;
    if (ret < 0.001) {
        // El plano esta atras de la camara
        return FLT_MAX;
    }

    return ret;
};


