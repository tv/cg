#ifndef GLWIDGET_H
#define GLWIDGET_H

#define GL_GLEXT_PROTOTYPES true

#include "GL/gl.h"
#include "GL/glu.h"
#include "GL/glext.h"

#include <iostream>
#include <QtOpenGL>
#include <QGLShaderProgram>
#include <QtGui/QMouseEvent>
#include <QTimer>
#include "qlobject.h"

#include "fileobject.h"
#include "light.h"

#include "material/materialtest.h"
#include "camera.h"

class GLWidget : public QGLWidget {
    Q_OBJECT

public:
    GLWidget(QWidget *parent = NULL);
protected:
    void initializeGL();
    void initializeObjects();
    void initializeFBO();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);

    qreal _screenWidth;
    qreal _screenHeight;

    void checkClick(QMouseEvent *event);

    QPoint lastPos;
    void _append(QLObject* obj);


    QGLShaderProgram* _program;

    void drawObjects(QGLShaderProgram * p, Camera *cam);


    qreal selectedIndex;
    QList<QLObject*> _objects;
    QList<Light*> _lights;

    QTimer *_timer;
    Camera *camera;


    int debugMode;

    // Shadow buffers

    QGLBuffer* blurVBO;
    QGLShaderProgram* _shadowProgram;
    QGLShaderProgram* _blurProgram;
    bool initializedFBO;
    float shadowMapCoef;
    float blurCoef;

    GLuint fboId;
    GLuint depthTextureId;
    GLuint colorTextureId;

    GLuint blurFboId;
    GLuint blurFboIdColorTextureId;
    void blurShadowMap(void);

    bool showDebug;
    bool blurEnabled;
    bool shadowEnabled;

    void generateShadowMap();

public slots:
    void animate();
signals:
    void animateChild();
};

#endif // GLWIDGET_H
