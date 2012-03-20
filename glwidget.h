#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <iostream>
#include <QtOpenGL>
#include <QtGui/QMouseEvent>
#include "qlobject.h"

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

    void checkClick(QMouseEvent *event);


    void drawFlag();

    QList<QLObject*> _objects;
};

#endif // GLWIDGET_H
