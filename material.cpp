#include "material.h"

Material::Material()
{
    this->loadTestMaterial();
}

void Material::loadTestMaterial()
{
    this->ambient = QVector4D(0.4f, 0.4f, 0.4f, 1.0f);
    this->specular = QVector4D(0.4f, 0.4f, 0.4f, 1.0f);
    this->diffuse = QVector4D(0.8f, 0.8f, 0.8f, 1.0f);
    this->emission = QVector4D(0.8f, 0.8f, 0.8f, 1.0f);
    this->shininess = 40.0f;
}

void Material::injectToShader(QGLShaderProgram* p)
{
    GLfloat mat[] = {
        this->ambient.x(),  this->ambient.y(),  this->ambient.z(),  this->ambient.w(),
        this->diffuse.x(),  this->diffuse.y(),  this->diffuse.z(),  this->diffuse.w(),
        this->emission.x(), this->emission.y(), this->emission.z(), this->emission.w(),
        this->specular.x(), this->specular.y(), this->specular.z(), this->specular.w(),
        this->shininess
    };

    p->setUniformValueArray("in_material", mat, 17, 1);
}

void Material::setAmbient(QVector4D in)
{
    this->ambient = in;
}

void Material::setSpecular(QVector4D in)
{
    this->specular = in;
}

void Material::setDiffuse(QVector4D in)
{
    this->diffuse = in;
}

void Material::setEmission(QVector4D in)
{
    this->emission = in;
}

void Material::setShihiness(GLfloat in)
{
    this->shininess = in;
}


