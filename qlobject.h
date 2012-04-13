#ifndef QLOBJECT_H
#define QLOBJECT_H

#include <QObject>
#include <QVector3D>
#include <QtOpenGL>
#include <iostream>
#include "material.h"

class QLObject : public QObject
{
Q_OBJECT
public:
    explicit QLObject(QObject *parent = 0);
    void setPosition(const QVector3D&);
    void setPosition(const qreal x, const qreal y);

    void setScale(qreal value);
    void setRotateX(qreal angle);
    void setRotateY(qreal angle);
    void setRotateZ(qreal angle);

    void addRotate(qreal x, qreal y, qreal z);
    void addRotateX(qreal angle);
    void addRotateY(qreal angle);
    void addRotateZ(qreal angle);

    bool ignoreClickEvent;

    virtual void draw();
    virtual void initialize();

    void addDepth(qreal depth);
    void setDepth(qreal depth);
    void setMaterial(Material material);
    qreal getDepth();

    qreal isHit(QVector3D position, qreal max);

    void drawBoundingBox();

protected:
    QVector3D _position;
    qreal _scale;

    qreal _rotationX;
    qreal _rotationY;
    qreal _rotationZ;

    Material _material;

    void vertex(QVector3D v1);
    void vertex(qreal x, qreal y);
    void vertex(qreal x, qreal y, qreal z);
    void color(qreal r, qreal g, qreal b);

    void surface(QVector3D v1, QVector3D v2, QVector3D v3);
    void surface(QVector3D v1, QVector3D v2, QVector3D v3, QVector3D v4);

    void rotate();
    void move();

    QVector3D _boundingBoxMax;
    QVector3D _boundingBoxMin;


    GLfloat no_mat[];
    GLfloat mat_ambient[];
    GLfloat mat_ambient_color[];
    GLfloat mat_diffuse[];
    GLfloat mat_specular[];
    GLfloat no_shininess[];
    GLfloat low_shininess[];
    GLfloat high_shininess[];
    GLfloat mat_emission[];
signals:
    void redraw();
    void rotationChanged();
    void positionChanged();
    void scaleChanged();
public slots:
    virtual void clicked();

};

#endif // QLOBJECT_H
