#include "FreeImage.h"
#include "include/Esfera.h"
#include "include/Point.h"
#include "include/Rayo.h"
#include "include/Color.h"
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



Color* rayTracing(int i, int j, list<Esfera*> objetos){

    Point* origen = new Point(0, 0, -1000);
    Point* direccion = new Point(i, j, 1250);
    direccion->normalizar();
    Rayo* rayo = new Rayo(origen, direccion);

    Color* color;

    for (Esfera* objeto : objetos) {
        if ((*objeto).intersectar(rayo) != FLT_MAX){
            color = new Color(0,150,0);
        }
        else{
            color = new Color(0,0,0);
        }
    }

    return color;
}

int main() {

    int Height = 500;
    int Width = 500;

    Esfera* esfera = new Esfera(new Point(200,250,220), 50);
    list<Esfera*> objetos;
    objetos.push_back(esfera);

	//list<Lightsource> luces;



    FIBITMAP *bitmap = FreeImage_Allocate(Width, Height, 32);

    string date = getDate();
    string path = "fotos/" + date + ".bmp";

    for (int i = 0; i < Height; i++) {
        for (int j = 0; j < Width; j++)  {

        Color* color = rayTracing(i, j, objetos);


        // CAMBIAAAAAAAAAAAAAR
        // CAMBIAAAAAAAAAAAAAR
        // CAMBIAAAAAAAAAAAAAR
        RGBQUAD colorQuad;
        colorQuad.rgbRed = color->getR();
        colorQuad.rgbGreen = color->getG();
        colorQuad.rgbBlue = color->getB();
        FreeImage_SetPixelColor(bitmap, i, j, &colorQuad);
        }
    }

    FreeImage_Save(FIF_BMP, bitmap, path.c_str(), 0);

    return 0;
}
