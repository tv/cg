#include "materialtest.h"

MaterialTest::MaterialTest()
{
    this->ambient[0] = 1.0f;
    this->ambient[1] = 0.0f;
    this->ambient[2] = 0.0f;
    this->ambient[3] = 1.0f;

    this->specular[0] = 1.0f;
    this->specular[1] = 0.0f;
    this->specular[2] = 0.0f;
    this->specular[3] = 1.0f;

    this->diffuse[0] = 1.0f;
    this->diffuse[1] = 0.0f;
    this->diffuse[2] = 0.0f;
    this->diffuse[3] = 1.0f;

    this->emission[0] = 0.0f;
    this->emission[1] = 0.0f;
    this->emission[2] = 0.0f;
    this->emission[3] = 0.0f;

    this->shininess = 3.0f;
}
