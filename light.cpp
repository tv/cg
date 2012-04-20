#include "light.h"

Light::Light(QObject *parent) :
    QObject(parent)
{
    this->position = QVector4D(12.0, 10.0, -22.0, 1.0);
    this->diffuse = QVector4D(1.0, 1.0, 1.0, 1.0);
    this->specular = QVector4D(1.0, 1.0, 1.0, 1.0);

    this->attenuations = QVector3D(0.0, 0.01, 0.0);
    this->spotCutOff = 70.0;
    this->spotExponent = 1.0;

    this->direction = QVector3D(-0.3, -1.0, 0.0);
    this->direction.normalize();

    this->angle = 0.0f;
}

void Light::outputFormat(QGLShaderProgram *p, QString name) {
    GLfloat mat[20] = {
        this->position.x(), this->position.y(), this->position.z(), this->position.w(),
        this->diffuse.x(), this->diffuse.y(), this->diffuse.z(), this->diffuse.w(),
        this->specular.x(), this->specular.y(), this->specular.z(), this->specular.w(),

        this->attenuations.x(), this->attenuations.y(), this->attenuations.z(),
        this->spotCutOff,
        this->spotExponent,

        this->direction.x(), this->direction.y(), this->direction.z()
    };

    const char *ptr = name.toAscii().data();
    p->setUniformValueArray(ptr, mat, 21, 1);
}

void Light::animate()
{
    this->angle += 5.0f;

    //this->direction.setX(sin(this->angle/180));
    //this->direction.setZ(cos(this->angle/180));

    this->direction.normalize();
}
