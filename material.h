#ifndef MATERIAL_H
#define MATERIAL_H
#include <QObject>
#include <QtOpenGL>
class Material
{
public:
    Material();
    void inject();

protected:
    GLfloat _ambient[4];
    GLfloat _diffuse[4];
    GLfloat _emission[4];
    GLfloat _specular[4];

    GLfloat _shininess;
};

#endif // MATERIAL_H
