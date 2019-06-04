#include "FreeImage.h"
#include "include/Esfera.h"
#include "include/Point.h"
#include "include/Rayo.h"
#include "include/Color.h"
#include "include/Luz.h"

#include <chrono>
#include <ctime>
#include <string>
#include <list>

#include <iostream>
#include <fstream>

using namespace std;

int writeFile(string txt)
{
  std::ofstream outfile;
  outfile.open("test.txt", std::ios_base::app);
  outfile << txt;
  return 0;
}


string getDate(){
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",timeinfo);
    string str(buffer);

    str[10] = '_';
    str[13] = '-';

    return str.substr(0,16);
}

Color* sombra_RR(Esfera* masCercano, Rayo* rayo, float distancia, Point* normal, list<Esfera*> objetos, list<Luz*> luces, int profundidad){


    Point* interseccion = (*rayo->getDireccion()) * distancia;
    Color* res = masCercano->getColor();

    float iluminacion = 0;

    for (Luz* luz : luces) {
        Point* direccionLuz = (*interseccion) - luz->getPosicion();

        // SACAR ESTO SI QUEREMOS QUE LA LUZ BAJE CUANDO SE ALEJA EL OBJETO
        direccionLuz->normalizar();

        // VER SI EL RAYO NO TENDRIA QUE ESTAR AL REVES
        Rayo* rayoLuz = new Rayo(luz->getPosicion(), direccionLuz);

        float cantidadLuz = 1;

        for (Esfera* objeto : objetos) {
            if (objeto != masCercano){
                float dist = objeto->intersectar(rayoLuz);
                if (dist < distancia)
                    cantidadLuz *= 0.2; // MULTIPLICAR POR COEFICIENTE
            }
        }

        float iluminacionEstaLuz = normal->dotProduct(direccionLuz);

        if (iluminacionEstaLuz * cantidadLuz > 0)
            iluminacion += iluminacionEstaLuz * cantidadLuz;
    }

    if (iluminacion > 0.2)
        res = res->escalar(iluminacion);
    else
        res = res->escalar(0.2);

    return res;
}

Color* traza_RR(Rayo* rayo, list<Esfera*> objetos, list<Luz*> luces, int profundidad){

    Color* color = new Color(255,255,255);

    Esfera* masCercano = nullptr;
    float distancia = FLT_MAX;

    for (Esfera* objeto : objetos) {
        float distObj = (*objeto).intersectar(rayo);
        if (distObj < distancia){
            masCercano = objeto;
            distancia = distObj;
        }
    }

    if (masCercano != nullptr){
        Point* interseccion = (*rayo->getDireccion()) * distancia;
        Point* normal = masCercano->getNormal(interseccion);
        return sombra_RR(masCercano, rayo, distancia, normal, objetos, luces, profundidad);
    }
    return color;
}



int main() {

    float Height = 500;
    float Width = 500;

    Esfera* esfera1 = new Esfera(new Point(0,0,8), 3, new Color(150,0,0));
    Esfera* esfera2 = new Esfera(new Point(1,1,4), 0.5, new Color(0,150,0));
    Luz* luz1 = new Luz(new Point(3,3,3), new Color(255,255,255), 100);

    list<Esfera*> objetos;
    objetos.push_back(esfera1);
    objetos.push_back(esfera2);

	list<Luz*> luces;
    luces.push_back(luz1);

    FIBITMAP *bitmap = FreeImage_Allocate(Width, Height, 32);

    string date = getDate();
    string path = "fotos/" + date + ".bmp";

    for (int i = 0; i < Height; i++) {
        for (int j = 0; j < Width; j++)  {

        Point* origen = new Point(0, 0, 0);
        Point* direccion = new Point((float)(i) / Height - 0.5, (float)(j) / Width - 0.5, 1);
        direccion->normalizar();
        Rayo* rayo = new Rayo(origen, direccion);

        Color* color = traza_RR(rayo, objetos, luces, 1);


        // CAMBIAAAAAAAAAAAAAR
        // CAMBIAAAAAAAAAAAAAR
        // CAMBIAAAAAAAAAAAAA
        RGBQUAD colorQuad;
        colorQuad.rgbRed = (int)color->getR();
        colorQuad.rgbGreen = (int)color->getG();
        colorQuad.rgbBlue = (int)color->getB();
        FreeImage_SetPixelColor(bitmap, i, j, &colorQuad);
        }
    }

    FreeImage_Save(FIF_BMP, bitmap, path.c_str(), 0);

    return 0;
}
