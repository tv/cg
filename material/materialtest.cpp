#include "materialtest.h"

MaterialTest::MaterialTest()
{
    this->_ambient[0] = 1.0f;
    this->_ambient[1] = 0.0f;
    this->_ambient[2] = 0.0f;
    this->_ambient[3] = 1.0f;

    this->_specular[0] = 1.0f;
    this->_specular[1] = 0.0f;
    this->_specular[2] = 0.0f;
    this->_specular[3] = 1.0f;

    this->_diffuse[0] = 1.0f;
    this->_diffuse[1] = 0.0f;
    this->_diffuse[2] = 0.0f;
    this->_diffuse[3] = 1.0f;

    this->_emission[0] = 0.0f;
    this->_emission[1] = 0.0f;
    this->_emission[2] = 0.0f;
    this->_emission[3] = 0.0f;

    this->_shininess = 3.0f;
}
