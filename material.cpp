#include "material.h"

Material::Material()
{
    this->ambient[0] = 0.4f;
    this->ambient[1] = 0.4f;
    this->ambient[2] = 0.4f;
    this->ambient[3] = 1.0f;

    this->specular[0] = 0.4f;
    this->specular[1] = 0.4f;
    this->specular[2] = 0.4f;
    this->specular[3] = 1.0f;

    this->diffuse[0] = 1.0f;
    this->diffuse[1] = 0.4f;
    this->diffuse[2] = 0.4f;
    this->diffuse[3] = 1.0f;

    this->emission[0] = 0.0f;
    this->emission[1] = 0.0f;
    this->emission[2] = 0.0f;
    this->emission[3] = 0.0f;

    this->shininess = 40.0f;
}

void Material::injectToShader(QGLShaderProgram* p)
{
    GLfloat mat[] = {
        this->ambient[0],  this->ambient[1],  this->ambient[2],  this->ambient[3],
        this->diffuse[0],  this->diffuse[1],  this->diffuse[2],  this->diffuse[3],
        this->emission[0], this->emission[1], this->emission[2], this->emission[3],
        this->specular[0], this->specular[1], this->specular[2], this->specular[3],
        this->shininess
    };

    p->setUniformValueArray("in_material", mat, 17, 1);
}
