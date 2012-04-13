#include "material.h"

Material::Material()
{
    this->_ambient[0] = 1.0f;
    this->_ambient[1] = 1.0f;
    this->_ambient[2] = 1.0f;
    this->_ambient[3] = 1.0f;

    this->_specular[0] = 1.0f;
    this->_specular[1] = 1.0f;
    this->_specular[2] = 1.0f;
    this->_specular[3] = 1.0f;

    this->_diffuse[0] = 1.0f;
    this->_diffuse[1] = 1.0f;
    this->_diffuse[2] = 1.0f;
    this->_diffuse[3] = 1.0f;

    this->_emission[0] = 1.0f;
    this->_emission[1] = 0.0f;
    this->_emission[2] = 0.0f;
    this->_emission[3] = 1.0f;

    this->_shininess = 40.0f;
}

void Material::inject()
{
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, this->_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, this->_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, this->_ambient);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, this->_shininess);

    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, this->_emission);
}
