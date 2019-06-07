#include "FreeImage.h"
#include "include/Esfera.h"
#include "include/Objeto.h"
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

Point* reflejar(Point* rayoLuz, Point* normal){
    float factor = 2 * normal->dotProduct(rayoLuz);
    Point* reflejado = (*normal) * factor;
    reflejado = (*reflejado) - rayoLuz;
    return reflejado;
}

Color* sombra_RR(Objeto* masCercano, Rayo* rayo, float distancia, Point* normal, list<Objeto*> objetos, list<Luz*> luces, int profundidad){
    Point* interseccion = (*rayo->getDireccion()) * distancia;
    Color* colorAmbiente = masCercano->getColor()->escalar(0.1);
    Color* colorDifuso = new Color(0,0,0);
    Color* colorEspecular = new Color(0,0,0);

    const float factorEspecular = 25;

    for (Luz* luz : luces) {
        Point* direccionLuz = (*interseccion) - luz->getPosicion();
        direccionLuz->normalizar();

        float prodInterno = normal->dotProduct(direccionLuz);

        Point* luzReflejada = reflejar((*direccionLuz) * -1, normal);
        float prodInternoReflejado = pow(luzReflejada->dotProduct((*rayo->getDireccion()) * -1)  , factorEspecular);

        if (prodInterno > 0){

            Rayo* rayoLuzObjeto = new Rayo(luz->getPosicion(), direccionLuz);
            // Si el factor es 1 llega toda la luz, si es 0 no llega nada (hay otro objeto opaco en el medio)
            float factorR = 1;
            float factorG = 1;
            float factorB = 1;

            // Chequear si hay objetos en el medio
            for (Objeto* objeto : objetos) {
                if (objeto != masCercano){
                    float dist = objeto->intersectar(rayoLuzObjeto);
                    if (dist < distancia){
                        factorR *= 1 - objeto->getOpacidadR();
                        factorG *= 1 - objeto->getOpacidadG();
                        factorB *= 1 - objeto->getOpacidadB();
                    }
                }
            }

            Point* vectorLuzInterseccion = (*luz->getPosicion()) - interseccion;
            float distanciaLuz = vectorLuzInterseccion->magnitude();

            Color* colorDifusoEstaLuz = new Color(luz->getColor()->getR() * masCercano->getColor()->getR() * factorR * prodInterno / (pow(distanciaLuz,2)),
                                            luz->getColor()->getG() * masCercano->getColor()->getG() * factorG * prodInterno / (pow(distanciaLuz,2)),
                                            luz->getColor()->getB() * masCercano->getColor()->getB() * factorB * prodInterno / (pow(distanciaLuz,2)));

            Color* colorEspecularEstaLuz = new Color(0,0,0);
            if (prodInternoReflejado > 0){
                colorEspecularEstaLuz = new Color(luz->getColor()->getR() * prodInternoReflejado * factorR,
                                                  luz->getColor()->getG() * prodInternoReflejado * factorG,
                                                  luz->getColor()->getB() * prodInternoReflejado* factorB);
            }

            colorDifuso = (*colorDifuso) + colorDifusoEstaLuz;
            colorEspecular = (*colorEspecular) + colorEspecularEstaLuz;
        }
    }


    /* DEBUG: USAR CON LA PELOTA EN (0,0,8) Y RADIO 3
    if(interseccion->getZ() - 5 < 0.000001 && 5 - interseccion->getZ() < 0.000001){
        //writeFile("\nRES: " + to_string(interseccion->getX()) + " " + to_string(interseccion->getY()) + " " + to_string(interseccion->getZ()));
        //writeFile("\nLuz1: " + to_string(prodInterno));
        //writeFile("\nLuz2: " + to_string(prodInternoReflejado));
        //writeFile("\n\nDirLuz: " + to_string(direccionLuz->getX()) + " " + to_string(direccionLuz->getY()) + " " + to_string(direccionLuz->getZ()));
        //writeFile("\n\nDirRef: " + to_string(luzReflejada->getX()) + " " + to_string(luzReflejada->getY()) + " " + to_string(luzReflejada->getZ()));
        writeFile("\n\nAmbiente: " + to_string(colorAmbiente->getR()) + " " + to_string(colorAmbiente->getG()) + " " + to_string(colorAmbiente->getB()));
        writeFile("\nDifuso: " + to_string(colorDifuso->getR()) + " " + to_string(colorDifuso->getG()) + " " + to_string(colorDifuso->getB()));
        writeFile("\nEspec: " + to_string(colorEspecular->getR()) + " " + to_string(colorEspecular->getG()) + " " + to_string(colorEspecular->getB()));
    }
    */

    Color* res = (*colorAmbiente) + colorDifuso;
    res = (*res) + colorEspecular;
    res->truncar();
    return res;
}

Color* traza_RR(Rayo* rayo, list<Objeto*> objetos, list<Luz*> luces, int profundidad){

    Color* color = new Color(0,0,0);
    rayo->getDireccion()->normalizar();
    Objeto* masCercano = nullptr;
    float distancia = FLT_MAX;

    for (Objeto* objeto : objetos) {
        float distObj = (*objeto).intersectar(rayo);
        if (distObj < distancia){
            masCercano = objeto;
            distancia = distObj;
        }
    }

    if (masCercano != nullptr){
        Point* interseccion = (*rayo->getDireccion()) * distancia;
        Point* normal = masCercano->getNormal(interseccion);
        normal->normalizar();
        return sombra_RR(masCercano, rayo, distancia, normal, objetos, luces, profundidad);
    }
    return color;
}

int main() {

    float Height = 500;
    float Width = 500;

    Objeto* esfera1 = new Esfera(new Point(0,0,8), 3, new Color(30,0,0), 1,1,1);
    Objeto* esfera2 = new Esfera(new Point(1,0.5,4), 0.5, new Color(0,20,0), 1,1,1);
    Luz* luz1 = new Luz(new Point(1,1,0), new Color(200,200,200));

    list<Objeto*> objetos;
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
