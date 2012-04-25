#ifndef LIGHT_H
#define LIGHT_H

#define GL_GLEXT_PROTOTYPES true

#include "GL/gl.h"
#include "GL/glu.h"
#include "GL/glext.h"

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

    void setCamera(Camera* cam);
    Camera* camera;

    void updateProjection(double aspect);

    bool initializedFBO;
    bool initializingFBO;
    void initializeShadowFBO(QSize screen);
    void bindDebugShadowMap();
    void bindFBO();
    void releaseFBO();


protected:
    GLuint fboId;
    GLuint depthTextureId;
signals:

    void redraw();
public slots:
    void animate();

};

#endif // LIGHT_H

