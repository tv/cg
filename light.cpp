#include "light.h"

Light::Light(QObject *parent) :
    QObject(parent)
{
    this->diffuse = QVector4D(1.0, 1.0, 1.0, 1.0);
    this->specular = QVector4D(1.0, 1.0, 1.0, 1.0);

    this->attenuations = QVector3D(0.0, 0.01, 0.0);
    this->spotCutOff = 180.0;
    this->spotExponent = 1.0;

    this->simpleLight = 0.0;
    this->angle = 0.0f;
}

void Light::injectToShader(QGLShaderProgram *p, int key) {
    GLfloat mat[20] = {
        this->camera->position.x(), this->camera->position.y(), this->camera->position.z(),
        this->simpleLight,
        this->diffuse.x(), this->diffuse.y(), this->diffuse.z(), this->diffuse.w(),
        this->specular.x(), this->specular.y(), this->specular.z(), this->specular.w(),

        this->attenuations.x(), this->attenuations.y(), this->attenuations.z(),
        this->spotCutOff,
        this->spotExponent,

        this->camera->direction.x(), this->camera->direction.y(), this->camera->direction.z()
    };

    QString name = QString("in_light%1").arg(key);

    const char *ptr = name.toAscii().data();
    p->setUniformValueArray(ptr, mat, 21, 1);

    this->camera->injectToShader(p, name);
}

void Light::setCamera(Camera* cam)
{
    this->camera = cam;
}

void Light::animate()
{
    //this->angle += 5.0f;

    //this->direction.setX(sin(this->angle/180));
    //this->direction.setZ(cos(this->angle/180));

    //this->direction.normalize();
}
