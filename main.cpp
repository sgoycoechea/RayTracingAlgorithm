#include "FreeImage.h"
#include "include/Esfera.h"
#include "include/Cilindro.h"
#include "include/Plano.h"
#include "include/Triangulo.h"
#include "include/Objeto.h"
#include "include/Point.h"
#include "include/Rayo.h"
#include "include/Color.h"
#include "include/Luz.h"
#include "tinyxml2/tinyxml2.h"


#include <chrono>
#include <ctime>
#include <string>
#include <list>
#include <vector>
#include <algorithm>


#include <iostream>
#include <fstream>

using namespace std;
using namespace tinyxml2;

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

Point reflejar(Point rayoLuz, Point normal){
    float factor = 2 * normal.dotProduct(rayoLuz);
    Point reflejado = normal * factor;
    reflejado = reflejado - rayoLuz;
    return reflejado;
}
/*
bool reflexionInternaTotal(Rayo* rayo, Point normal, float n1, float n2){
    if (n1 > n2){
        float prodInterno = normal.dotProduct(rayo->getDireccion());
        float ang = acos(prodInterno);
        return ang > asin(n2/n1);
    } else return false;
}
*/
vector<Color> traza_RR(Rayo* rayo, list<Objeto*> objetos, list<Luz*> luces, vector<Objeto*> objetosAtravezados, int profundidad);


Color sombra_RR(Objeto* masCercano, Rayo* rayo, float distancia, Point normal, list<Objeto*> objetos, list<Luz*> luces, vector<Objeto*> objetosAtravezados,int profundidad){
    Point interseccion = rayo->getOrigen() + (rayo->getDireccion() * distancia);

    Color colorAmbiente = masCercano->getColor().escalar(masCercano->getCoefAmbiente());
    Color colorDifuso = Color(0,0,0);
    Color colorEspecular = Color(0,0,0);
    Color colorRefraccion = Color(0,0,0);
    Color colorReflexion = Color(0,0,0);
    const float factorN = 25;

    for (Luz* luz : luces) {
        Point direccionLuz = interseccion - luz->getPosicion();
        direccionLuz.normalizar();

        float prodInterno = normal.dotProduct(direccionLuz);

        if (prodInterno > 0){

            Point vectorLuzObjeto = luz->getPosicion() - interseccion;
            Rayo* rayoLuzObjeto = new Rayo(luz->getPosicion(), direccionLuz);
            float distanciaLuz = vectorLuzObjeto.magnitude();

            // Si el factor es 1 llega toda la luz, si es 0 no llega nada (hay otro objeto opaco en el medio)
            float factorDifuso = masCercano->getCoefDifuso();
            float factorEspecular = masCercano->getCoefEspecular();

            // Chequear si hay objetos en el medio
            for (Objeto* objeto : objetos) {
                if (objeto != masCercano){
                    float dist = objeto->intersectar(rayoLuzObjeto);
                    if (dist + 0.001 < distanciaLuz){
                        factorDifuso *= objeto->getCoefTransmision(); //no es 1 - ... ?? xq ahora cuando la transmision = 1 no se escala el factor
                        factorEspecular *= objeto->getCoefTransmision();
                    }
                }
            }

            // VER SI ESTA BIEN MULTIPLICAR POR factorR,G,B Y VER SI NO HAY QUE MULTIPLICAR POR masCercano->getOpacidadR()

            // Luz difusa
            Color colorDifusoEstaLuz = Color(luz->getColor().getR() * masCercano->getColor().getR() * factorDifuso * prodInterno / (pow(distanciaLuz,2)),
                                            luz->getColor().getG() * masCercano->getColor().getG() * factorDifuso * prodInterno / (pow(distanciaLuz,2)),
                                            luz->getColor().getB() * masCercano->getColor().getB() * factorDifuso * prodInterno / (pow(distanciaLuz,2)));

            // Luz especular
            Point luzReflejada = reflejar(direccionLuz * -1, normal);
            float prodInternoReflejado = pow(luzReflejada.dotProduct(rayo->getDireccion() * -1)  , factorN);
            Color colorEspecularEstaLuz = Color(0,0,0);
            if (prodInternoReflejado > 0){
                colorEspecularEstaLuz = Color(luz->getColor().getR() * prodInternoReflejado * factorEspecular,
                                                  luz->getColor().getG() * prodInternoReflejado * factorEspecular,
                                                  luz->getColor().getB() * prodInternoReflejado* factorEspecular);
            }

            colorDifuso = colorDifuso + colorDifusoEstaLuz;
            colorEspecular = colorEspecular + colorEspecularEstaLuz;
        }
    }

    if (profundidad < 5){

        if (masCercano->getCoefEspecular() > 0){

            Point direcReflejada = reflejar(rayo->getDireccion() * -1, normal);
            direcReflejada.normalizar();
            Point interseccion = (rayo->getOrigen()) + (rayo->getDireccion() * (distancia - 0.0001));

            Rayo* rayo_r = new Rayo(interseccion, direcReflejada);
            Color color_r = (traza_RR (rayo_r, objetos, luces, objetosAtravezados, profundidad + 1)).back();

            colorReflexion = color_r.escalar(masCercano->getCoefEspecular()) ;
        }

        if( masCercano->getCoefTransmision() > 0 ){

            normal.normalizar();
            rayo->getDireccion().normalizar();

            Point interseccion = rayo->getOrigen() + (rayo->getDireccion() * (distancia + 0.0001));
            float n1, n2;
/*
            Busco en el stack si ya esta el objeto con el que intersecté

                Si no está
                     n1 = 1 si el stack esta vacio, sino el que esta en la cima del stack
                     n2 = el indice del objeto intersectado
                     inserto

                Si está
                     n1 = al que está en la cima
                     saco del stack el objeto intersectado
                     n2 = 1 si el stack esta vacio, sino el que esta en la cima del stack
                     N = -N
*/


            vector<Objeto*>::iterator itr = find(objetosAtravezados.begin(), objetosAtravezados.end(), masCercano);

            if (itr != objetosAtravezados.end()){
            //if (find(objetosAtravezados.begin(), objetosAtravezados.end(), masCercano) != objetosAtravezados.end()) {

                //n1 = tope del stack
                n1 = objetosAtravezados.back()->getIndiceRefraccion();
                //saco del stack objeto intersectado
                objetosAtravezados.erase(itr);

                if (objetosAtravezados.empty())
                    n2 = 1;
                else
                    //n2 = tope del stack
                    n2 = objetosAtravezados.back()->getIndiceRefraccion();

                normal = normal * -1;

            }else{
                if (objetosAtravezados.empty())
                    n1 = 1;
                else
                    //n1 = tope del stack
                    n1 = objetosAtravezados.back()->getIndiceRefraccion();

                n2 = masCercano->getIndiceRefraccion();

                objetosAtravezados.push_back(masCercano);
            }


            Point N = normal;
            Point I = rayo->getDireccion();
            N.normalizar();
            I.normalizar();


            //float ang1 = acos(N.dotProduct(I)); //es -I??
            //float ang2 = asin(sin(ang1) * n1 / n2);

            float ang1 = acos ( N.dotProduct(I * -1));
            //float ang2 = asin( n1*sin(ang1)/n2 );

            float angCritico = asin(n1/n2);


            // if (!reflexionInternaTotal(rayo, normal, n1, n2)){
            if(!(n1>n2 && ang1> angCritico)){

                //inmensia.com
                float n = n1 / n2;
                Point direcRefractada = I /n + N * ( (ang1/n) - sqrt(1 + (pow(ang1,2) - 1) / pow(n, 2) ));
                direcRefractada.normalizar();
             //ECUACIONES VIEJAS
/*
                float ang1 = acos(normal.dotProduct(rayo->getDireccion()) );
                float ang2 = asin( sin(ang1)* n1 / n2);
                Point M = (*(*rayo->getDireccion() + (*normal * cos(ang1)))) / sin(ang1);
                Point A = *M * sin(ang2);
                Point B = *normal * -cos(ang2);

                Point direcRefractada = *A + B;
*/



                Rayo* rayo_t = new Rayo(interseccion, direcRefractada);
                Color color_t = (traza_RR (rayo_t, objetos, luces, objetosAtravezados, profundidad + 1)).back();

                colorRefraccion = color_t.escalar(masCercano->getCoefTransmision()) ;
                delete rayo_t;

            }else{
                Point direcReflejado = reflejar(rayo->getDireccion() * -1, normal);
                direcReflejado.normalizar();
                Rayo* rayo_Reflejado = new Rayo(interseccion, direcReflejado); //es interseccion lo que hay q poner?
                Color color_t = (traza_RR (rayo_Reflejado, objetos, luces, objetosAtravezados, profundidad + 1)).back();
                colorRefraccion = color_t.escalar(masCercano->getCoefTransmision()) ;
                delete rayo_Reflejado;

            }

        }

    }

    Color res = colorAmbiente + colorDifuso;
    res = res + colorEspecular;
    res = res + colorRefraccion;
    res = res + colorReflexion;
    res.truncar();
    return res;
}

vector<Color> traza_RR(Rayo* rayo, list<Objeto*> objetos, list<Luz*> luces, vector<Objeto*> objetosAtravezados, int profundidad){

    Color color = Color(0,0,0);
    float coefT = 0;
    float coefR = 0;
    Color colorT = Color(250,250,250);
    Color colorR = Color(250,250,250);
    rayo->getDireccion().normalizar();
    Objeto* masCercano = nullptr;
    float distancia = FLT_MAX;

    for (Objeto* objeto : objetos) {
        float distObj = (*objeto).intersectar(rayo);
        if (distObj < distancia + 0.001){
            distancia = distObj;
            masCercano = objeto;
            color = (*objeto).getColor();

            coefT = (*objeto).getCoefTransmision();
            coefR = (*objeto).getCoefEspecular();

        }
    }
    colorT = colorT.escalar(coefT);
    colorR = colorR.escalar(coefR);

    if (masCercano != nullptr){
        Point interseccion = rayo->getOrigen() + rayo->getDireccion() * (distancia + 0.0001);
        Point normal = masCercano->getNormal(interseccion);
        normal.normalizar();
        color = sombra_RR(masCercano, rayo, distancia, normal, objetos, luces, objetosAtravezados, profundidad);
    }
    vector<Color> colores;
    colores.push_back(colorT);
    colores.push_back(colorR);
    colores.push_back(color);

    return colores;
}

/*
vector<Color> traza_aux(Rayo* rayo, list<Objeto*> objetos){

    float coefT = 0;
    float coefR = 0;
    Color colorT = Color(250,250,250);
    Color colorR = Color(250,250,250);
    rayo->getDireccion().normalizar();
    Objeto* masCercano = nullptr;
    float distancia = FLT_MAX;

    for (Objeto* objeto : objetos) {
        float distObj = (*objeto).intersectar(rayo);
        if (distObj < distancia + 0.001){
            distancia = distObj;
            masCercano = objeto;

            coefT = (*objeto).getCoefTransmision();
            coefR = (*objeto).getCoefEspecular();

        }
    }

    colorT = colorT.escalar(coefT);
    colorR = colorR.escalar(coefR);

    vector <Color> colores;
    colores.push_back(colorT);
    colores.push_back(colorR);

    return colores;
}
*/

list<Objeto*> inicializarObjetos(){
    list<Objeto*> objetos;

    tinyxml2::XMLDocument doc;
    doc.LoadFile("escena.xml");

    // Esferas
    for(tinyxml2::XMLElement* child = doc.FirstChildElement("file")->FirstChildElement("objetos")->FirstChildElement("esferas")->FirstChildElement("esfera"); child != 0; child = child->NextSiblingElement())
    {

        float x = stof(child->ToElement()->Attribute("centroX"));
        float y = stof(child->ToElement()->Attribute("centroY"));
        float z = stof(child->ToElement()->Attribute("centroZ"));

        float r = stof(child->ToElement()->Attribute("colorR"));
        float g = stof(child->ToElement()->Attribute("colorG"));
        float b = stof(child->ToElement()->Attribute("colorB"));

        float rad = stof(child->ToElement()->Attribute("radio"));

        float coefTransmision = stof(child->ToElement()->Attribute("coefTransmision"));
        float coefEspecular = stof(child->ToElement()->Attribute("coefEspecular"));
        float coefDifuso = stof(child->ToElement()->Attribute("coefDifuso"));
        float coefAmbiente = stof(child->ToElement()->Attribute("coefAmbiente"));
        float indiceRefraccion = stof(child->ToElement()->Attribute("indiceRefraccion"));

        Objeto* esfera = new Esfera(Point(x,y,z), rad, Color(r,g,b), coefTransmision, coefEspecular, coefDifuso, coefAmbiente, indiceRefraccion);
        objetos.push_back(esfera);
    }

    // Cilindros

    for(tinyxml2::XMLElement* child = doc.FirstChildElement("file")->FirstChildElement("objetos")->FirstChildElement("cilindros")->FirstChildElement("cilindro"); child != 0; child = child->NextSiblingElement())
    {
        float centroX = stof(child->ToElement()->Attribute("centroBaseX"));
        float centroY = stof(child->ToElement()->Attribute("centroBaseY"));
        float centroZ = stof(child->ToElement()->Attribute("centroBaseZ"));

        float dirX = stof(child->ToElement()->Attribute("direccionX"));
        float dirY = stof(child->ToElement()->Attribute("direccionY"));
        float dirZ = stof(child->ToElement()->Attribute("direccionZ"));

        float rad = stof(child->ToElement()->Attribute("radio"));
        float alt = stof(child->ToElement()->Attribute("altura"));

        float r = stof(child->ToElement()->Attribute("colorR"));
        float g = stof(child->ToElement()->Attribute("colorG"));
        float b = stof(child->ToElement()->Attribute("colorB"));

        float coefTransmision = stof(child->ToElement()->Attribute("coefTransmision"));
        float coefEspecular = stof(child->ToElement()->Attribute("coefEspecular"));
        float coefDifuso = stof(child->ToElement()->Attribute("coefDifuso"));
        float coefAmbiente = stof(child->ToElement()->Attribute("coefAmbiente"));
        float indiceRefraccion = stof(child->ToElement()->Attribute("indiceRefraccion"));

        Objeto* cilindro = new Cilindro(Point(centroX,centroY,centroZ), Point(dirX,dirY,dirZ), rad, alt, Color(r,g,b), coefTransmision, coefEspecular, coefDifuso, coefAmbiente, indiceRefraccion);
        objetos.push_back(cilindro);
    }

    // Planos
    for(tinyxml2::XMLElement* child = doc.FirstChildElement("file")->FirstChildElement("objetos")->FirstChildElement("planos")->FirstChildElement("plano"); child != 0; child = child->NextSiblingElement())
    {
        float puntoX = stof(child->ToElement()->Attribute("puntoX"));
        float puntoY = stof(child->ToElement()->Attribute("puntoY"));
        float puntoZ = stof(child->ToElement()->Attribute("puntoZ"));

        float normalX = stof(child->ToElement()->Attribute("normalX"));
        float normalY = stof(child->ToElement()->Attribute("normalY"));
        float normalZ = stof(child->ToElement()->Attribute("normalZ"));

        float r = stof(child->ToElement()->Attribute("colorR"));
        float g = stof(child->ToElement()->Attribute("colorG"));
        float b = stof(child->ToElement()->Attribute("colorB"));

        float coefTransmision = stof(child->ToElement()->Attribute("coefTransmision"));
        float coefEspecular = stof(child->ToElement()->Attribute("coefEspecular"));
        float coefDifuso = stof(child->ToElement()->Attribute("coefDifuso"));
        float coefAmbiente = stof(child->ToElement()->Attribute("coefAmbiente"));
        float indiceRefraccion = stof(child->ToElement()->Attribute("indiceRefraccion"));

        Objeto* plano = new Plano(Point(puntoX,puntoY,puntoZ), Point(normalX,normalY,normalZ), Color(r,g,b), coefTransmision, coefEspecular, coefDifuso, coefAmbiente, indiceRefraccion);
        objetos.push_back(plano);
    }

    // Triangulos
    for(tinyxml2::XMLElement* child = doc.FirstChildElement("file")->FirstChildElement("objetos")->FirstChildElement("triangulos")->FirstChildElement("triangulo"); child != 0; child = child->NextSiblingElement())
    {
        float puntoA1 = stof(child->ToElement()->Attribute("puntoA1"));
        float puntoA2 = stof(child->ToElement()->Attribute("puntoA2"));
        float puntoA3 = stof(child->ToElement()->Attribute("puntoA3"));

        float puntoB1 = stof(child->ToElement()->Attribute("puntoB1"));
        float puntoB2 = stof(child->ToElement()->Attribute("puntoB2"));
        float puntoB3 = stof(child->ToElement()->Attribute("puntoB3"));

        float puntoC1 = stof(child->ToElement()->Attribute("puntoC1"));
        float puntoC2 = stof(child->ToElement()->Attribute("puntoC2"));
        float puntoC3 = stof(child->ToElement()->Attribute("puntoC3"));

        float r = stof(child->ToElement()->Attribute("colorR"));
        float g = stof(child->ToElement()->Attribute("colorG"));
        float b = stof(child->ToElement()->Attribute("colorB"));

        float coefTransmision = stof(child->ToElement()->Attribute("coefTransmision"));
        float coefEspecular = stof(child->ToElement()->Attribute("coefEspecular"));
        float coefDifuso = stof(child->ToElement()->Attribute("coefDifuso"));
        float coefAmbiente = stof(child->ToElement()->Attribute("coefAmbiente"));
        float indiceRefraccion = stof(child->ToElement()->Attribute("indiceRefraccion"));

        Objeto* triangulo = new Triangulo(Point(puntoA1,puntoA2,puntoA3), Point(puntoB1,puntoB2,puntoB3), Point(puntoC1,puntoC2,puntoC3), Color(r,g,b), coefTransmision, coefEspecular, coefDifuso, coefAmbiente, indiceRefraccion);
        objetos.push_back(triangulo);
    }

    return objetos;
}

list<Luz*> inicializarLuces(){
    list<Luz*> luces;

    tinyxml2::XMLDocument doc;
    doc.LoadFile("escena.xml");

    for(tinyxml2::XMLElement* child = doc.FirstChildElement("file")->FirstChildElement("luces")->FirstChildElement("luz"); child != 0; child = child->NextSiblingElement())
    {
        tinyxml2::XMLElement* posicionElement = child->FirstChildElement("posicion");
        tinyxml2::XMLElement* colorElement = child->FirstChildElement("color");

        float x = stoi(posicionElement->ToElement()->Attribute("x"));
        float y = stoi(posicionElement->ToElement()->Attribute("y"));
        float z = stoi(posicionElement->ToElement()->Attribute("z"));

        float r = stoi(colorElement->ToElement()->Attribute("r"));
        float g = stoi(colorElement->ToElement()->Attribute("g"));
        float b = stoi(colorElement->ToElement()->Attribute("b"));

        Luz* luz = new Luz(Point(x,y,z), Color(r,g,b));
        luces.push_back(luz);
    }

    return luces;
}


int main() {
    tinyxml2::XMLDocument doc;
    doc.LoadFile("escena.xml");

    // Tamaño de la imagen en pixeles
    tinyxml2::XMLElement* resolucionElement = doc.FirstChildElement("file")->FirstChildElement("resolucion");
    float Height = stoi(resolucionElement->ToElement()->Attribute("x"));
    float Width = stoi(resolucionElement->ToElement()->Attribute("y"));

    // Inicializar camara
    tinyxml2::XMLElement* child = doc.FirstChildElement("file")->FirstChildElement("camara");
    tinyxml2::XMLElement* posElement = child->FirstChildElement("posicion");
    tinyxml2::XMLElement* dirElement = child->FirstChildElement("direccion");
    tinyxml2::XMLElement* upElement = child->FirstChildElement("up");
    Point camaraPos = Point(stof(posElement->ToElement()->Attribute("x")), stof(posElement->ToElement()->Attribute("y")), stof(posElement->ToElement()->Attribute("z")));
    Point camaraDir = Point(stof(dirElement->ToElement()->Attribute("x")), stof(dirElement->ToElement()->Attribute("y")), stof(dirElement->ToElement()->Attribute("z")));
    Point camaraUp = Point(stof(upElement->ToElement()->Attribute("x")), stof(upElement->ToElement()->Attribute("y")), stof(upElement->ToElement()->Attribute("z")));

    FIBITMAP *bitmap = FreeImage_Allocate(Width, Height, 32);
    string date = getDate();
    string path = "fotos/" + date + ".bmp";

//img aux
    FIBITMAP *bitmapT = FreeImage_Allocate(Width, Height, 32);
    string t = "transmision";
    string pathT = "fotos/" + t  + ".bmp";

    FIBITMAP *bitmapR = FreeImage_Allocate(Width, Height, 32);
    string r = "reflexion";
    string pathR = "fotos/" + r  + ".bmp";

// img aux

    list<Luz*> luces = inicializarLuces();
    list<Objeto*> objetos = inicializarObjetos();
    vector<Objeto*> objetosAtravezados;

    Point direccionLateral = camaraDir.productoVectorial(camaraUp) * -1;
    camaraDir.normalizar();
    camaraUp.normalizar();
    direccionLateral.normalizar();


    for (int i = 0; i < Height; i++) {
        for (int j = 0; j < Width; j++)  {

        Point direccionRayo = camaraDir;
        direccionRayo = direccionRayo + (direccionLateral * (i / Height - 0.5)); // Mover horizontalmente
        direccionRayo = direccionRayo + (camaraUp * (j / Width - 0.5)); // Mover verticalmente
        direccionRayo.normalizar();
        Rayo* rayo = new Rayo(camaraPos, direccionRayo);

        vector<Color> colores = traza_RR(rayo, objetos, luces, objetosAtravezados, 0);
        Color color = colores.back();
        //Color color = (traza_RR(rayo, objetos, luces, objetosAtravezados, 0)).back();

        RGBQUAD colorQuad;
        colorQuad.rgbRed = (int)color.getR();
        colorQuad.rgbGreen = (int)color.getG();
        colorQuad.rgbBlue = (int)color.getB();
        FreeImage_SetPixelColor(bitmap, i, j, &colorQuad);

// para imagenes aux

        colores.pop_back();
        Color colorR = colores.back();

        RGBQUAD colorQuadR;
        colorQuadR.rgbRed = (int)colorR.getR();
        colorQuadR.rgbGreen = (int)colorR.getG();
        colorQuadR.rgbBlue = (int)colorR.getB();
        FreeImage_SetPixelColor(bitmapR, i, j, &colorQuadR);

        colores.pop_back();
        Color colorT = colores.back();

        RGBQUAD colorQuadT;
        colorQuadT.rgbRed = (int)colorT.getR();
        colorQuadT.rgbGreen = (int)colorT.getG();
        colorQuadT.rgbBlue = (int)colorT.getB();
        FreeImage_SetPixelColor(bitmapT, i, j, &colorQuadT);






// para imagenes aux



        }
    }


    FreeImage_Save(FIF_BMP, bitmap, path.c_str(), 0);

    FreeImage_Save(FIF_BMP, bitmapT, pathT.c_str(), 0);

    FreeImage_Save(FIF_BMP, bitmapR, pathR.c_str(), 0);

    return 0;
}
