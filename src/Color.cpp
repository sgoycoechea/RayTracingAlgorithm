#include "../include/Color.h"

using namespace std;

Color::Color(int r, int g, int b){
    this->r = r;
    this->g = g;
    this->b = b;
}

int Color::getR() {
  return r;
}

int Color::getG() {
  return g;
}

int Color::getB() {
  return b;
}
