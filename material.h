#ifndef MATERIAL_H
#define MATERIAL_H
#include <QObject>
#include <QtOpenGL>

//#include "glwidget.h"

class Material
{
public:
    Material();
    void injectToShader(QGLShaderProgram* p);

    GLfloat ambient[4];
    GLfloat diffuse[4];
    GLfloat emission[4];
    GLfloat specular[4];
    GLfloat shininess;
    QString name;

protected:


};

#endif // MATERIAL_H
