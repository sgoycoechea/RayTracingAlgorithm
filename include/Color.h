#ifndef COLOR_H
#define COLOR_H

using namespace std;

class Color{
    private:
        int r;
        int g;
        int b;
    public:
        Color(int, int, int);
        int getR();
        int getG();
        int getB();
};

#endif
