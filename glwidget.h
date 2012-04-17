#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <iostream>
#include <QtOpenGL>
#include <QGLShaderProgram>
#include <QtGui/QMouseEvent>
#include <QTimer>
#include "qlobject.h"

#include "fileobject.h"
#include "light.h"

#include "material/materialtest.h"

class GLWidget : public QGLWidget {
    Q_OBJECT

public:
    GLWidget(QWidget *parent = NULL);
protected:
    void initializeGL();
    void initializeObjects();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);

    QMatrix4x4 getPerspective();
    QMatrix4x4 getView();
    qreal _screenWidth;
    qreal _screenHeight;

    void checkClick(QMouseEvent *event);

    QPoint lastPos;
    void _append(QLObject* obj);

    QGLShaderProgram* _program;

    qreal selectedIndex;
    QList<QLObject*> _objects;

    QList<Light*> _lights;

    QTimer *_timer;
public slots:
    void animate();
signals:
    void animateChild();
};

#endif // GLWIDGET_H
