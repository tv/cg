#ifndef LIGHT_H
#define LIGHT_H

#include <QObject>
#include <QVector3D>
#include <QVector4D>
#include <QtOpenGL>

#include "camera.h"

class Light : public QObject
{
Q_OBJECT
public:
    explicit Light(QObject *parent = 0);

    void injectToShader(QGLShaderProgram *p, int key);

    QVector4D diffuse;
    QVector4D specular;

    QVector3D attenuations;

    float spotCutOff, spotExponent;

    float simpleLight;

    void setCamera(Camera* cam);
    Camera* camera;
protected:
    qreal angle;
signals:

    void redraw();
public slots:
    void animate();

};

#endif // LIGHT_H

