#include "FreeImage.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <string>
#include <fstream>


using namespace std;

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

int main(int argc, char *argv[]) {
    FIBITMAP *bitmap = FreeImage_Allocate(640, 240, 32);

    string date = getDate();
    string path = "fotos/" + date + ".bmp";

    FreeImage_Save(FIF_BMP, bitmap, path.c_str(), 0);

    return 0;
}
