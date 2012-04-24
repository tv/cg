#include "light.h"

Light::Light(QObject *parent) :
    QObject(parent)
{
    this->diffuse = QVector4D(1.0, 1.0, 1.0, 1.0);
    this->specular = QVector4D(1.0, 1.0, 1.0, 1.0);

    this->attenuations = QVector3D(1.0, 0.01, 0.0);
    this->spotCutOff = 180.0;
    this->spotExponent = 1.0;

    this->angle = 0.0f;
}

void Light::injectToShader(QGLShaderProgram *p, int key) {
    Camera* cam = this->camera;

    GLfloat mat[20] = {
        cam->position.x(), cam->position.y(), cam->position.z(), cam->position.w(),
        this->diffuse.x(), this->diffuse.y(), this->diffuse.z(), this->diffuse.w(),
        this->specular.x(), this->specular.y(), this->specular.z(), this->specular.w(),

        this->attenuations.x(), this->attenuations.y(), this->attenuations.z(),
        this->spotCutOff,
        this->spotExponent,

        cam->direction.x(), cam->direction.y(), cam->direction.z()
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

void Light::updateProjection(double aspect)
{
    this->camera->setPerspective(45.0f, aspect, 0.1, 100.0);
    //this->camera->setPerspective(this->spotCutOff/2, aspect, 0.1, 100.0);
}

void Light::animate()
{
    //this->angle += 5.0f;

    //this->direction.setX(sin(this->angle/180));
    //this->direction.setZ(cos(this->angle/180));

    //this->direction.normalize();
}
