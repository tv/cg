#include "material.h"

Material::Material()
{
    this->_ambient[0] = 0.4f;
    this->_ambient[1] = 0.4f;
    this->_ambient[2] = 0.4f;
    this->_ambient[3] = 1.0f;

    this->_specular[0] = 0.4f;
    this->_specular[1] = 0.4f;
    this->_specular[2] = 0.4f;
    this->_specular[3] = 1.0f;

    this->_diffuse[0] = 0.4f;
    this->_diffuse[1] = 0.4f;
    this->_diffuse[2] = 0.4f;
    this->_diffuse[3] = 1.0f;

    this->_emission[0] = 0.0f;
    this->_emission[1] = 0.0f;
    this->_emission[2] = 0.0f;
    this->_emission[3] = 0.0f;

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

void Material::injectToShader(QGLShaderProgram* p)
{
    /*
    p->enableAttributeArray("v_specular");
    p->enableAttributeArray("v_diffuse");
    p->enableAttributeArray("v_ambient");
    p->enableAttributeArray("v_emission");

    p->setAttributeArray("v_specular", GL_FLOAT, this->_specular,4);
    p->setAttributeArray("v_diffuse", GL_FLOAT, this->_diffuse, 4);
    p->setAttributeArray("v_ambient", GL_FLOAT, this->_ambient,4);
    p->setAttributeArray("v_emission", GL_FLOAT, this->_emission,4);
    p->setAttributeValue("f_shininess", this->_shininess);

    p->disableAttributeArray("v_specular");
    p->disableAttributeArray("v_diffuse");
    p->disableAttributeArray("v_ambient");
    p->disableAttributeArray("v_emission");*/


    GLfloat mat[] = {
        this->_ambient[0], this->_ambient[1], this->_ambient[2], this->_ambient[3],
        this->_diffuse[0], this->_diffuse[1], this->_diffuse[2], this->_diffuse[3],
        this->_emission[0], this->_emission[1], this->_emission[2], this->_emission[3],
        this->_specular[0], this->_specular[1], this->_specular[2], this->_specular[3],
        this->_shininess
    };

    p->setUniformValueArray("in_material", mat, 17, 1);
}
