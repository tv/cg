#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <iostream>
#include <QtOpenGL>
#include <QtGui/QMouseEvent>
#include "qlobject.h"

#include "fileobject.h"
#include "lightobject.h"
#include "textobject.h"
#include "flagobject.h"

class GLWidget : public QGLWidget {
    Q_OBJECT

public:
    GLWidget(QWidget *parent = NULL);
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);

    void rotateX(int rotate);
    void rotateY(int rotate);
    void rotateZ(int rotate);

    void checkClick(QMouseEvent *event);

    void addNewFlag();
    void deleteSelectedFlag();

    qreal nextDepth;


    QPoint lastPos;
    qreal selectedIndex;
    QList<QLObject*> _objects;

    QVector3D camera;
};

#endif // GLWIDGET_H
