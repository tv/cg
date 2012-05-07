#ifndef MATERIAL_H
#define MATERIAL_H

#define GL_GLEXT_PROTOTYPES true


#include <QObject>
#include <QtOpenGL>

//#include "glwidget.h"

class Material
{
public:
    Material();
    void injectToShader(QGLShaderProgram* p);

    void loadTestMaterial();

    void setAmbient(QVector4D in);
    void setSpecular(QVector4D in);
    void setDiffuse(QVector4D in);
    void setEmission(QVector4D in);
    void setShihiness(GLfloat in);

    void setImage(QString in);


protected:
    QVector4D ambient;
    QVector4D diffuse;
    QVector4D emission;
    QVector4D specular;
    GLfloat shininess;

    GLuint textureId;

};

#endif // MATERIAL_H
