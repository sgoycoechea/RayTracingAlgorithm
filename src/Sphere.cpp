#include "../include/Sphere.h"
#include <iostream>
#include <fstream>

using namespace std;

Esfera::Esfera(Point centro, double rad, Color color, float coefTransmision, float coefEspecular, float coefDifuso, float coefAmbiente, float indiceRefraccion):Objeto(color, coefTransmision, coefEspecular, coefDifuso, coefAmbiente, indiceRefraccion){
    this->centro = centro;
    this->rad = rad;
}

Point Esfera::getNormal(Point punto){
    return centro - (punto);
}

double Esfera::intersectar(Rayo* rayo) {
    double dirX = rayo->getDireccion().getX();
    double dirY = rayo->getDireccion().getY();
    double dirZ = rayo->getDireccion().getZ();
    double oriX = rayo->getOrigen().getX();
    double oriY = rayo->getOrigen().getY();
    double oriZ = rayo->getOrigen().getZ();

    //Bhaskaras
    double a = pow(dirX,2) + pow(dirY,2) + pow(dirZ,2);
    double b = 2 * ( dirX * (oriX - centro.getX()) + dirY * (oriY - centro.getY()) + dirZ * (oriZ - centro.getZ()) );
    double c = pow(oriX - centro.getX(), 2) + pow(oriY - centro.getY(), 2) + pow(oriZ - centro.getZ(), 2) - pow(rad, 2);
    double delta = b*b - 4 * a * c;


    if (delta < 0){
        return FLT_MAX; // No hay interseccion
    }

    double raiz1 = (-b - sqrt(delta)) / (2 * a);
    double raiz2 = (-b + sqrt(delta)) / (2 * a);

    if (raiz2 < 0.01)
        return FLT_MAX; // La esfera esta toda atras de la camara

    if (raiz1 > 0.01)
        return raiz1; // La esfera esta toda adelante de la camara
    else
        return raiz2; // La camara esta adentro de la esfera
};


