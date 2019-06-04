#ifndef COLOR_H
#define COLOR_H

using namespace std;

class Color{
    private:
        float r;
        float g;
        float b;
    public:
        Color(float, float, float);
        float getR();
        float getG();
        float getB();
        Color* escalar(float);
        Color* mezclar(Color*);
        Color* operator+(Color*);
};

#endif
