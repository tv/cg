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

    void setScale(qreal value);
    void setRotateX(qreal angle);
    void setRotateY(qreal angle);
    void setRotateZ(qreal angle);

    void addRotate(qreal x, qreal y, qreal z);
    void addRotateX(qreal angle);
    void addRotateY(qreal angle);
    void addRotateZ(qreal angle);

    bool ignoreClickEvent;

    virtual void injectToShader(QGLShaderProgram *p);


    QMatrix4x4 getTransformationMatrix();

    void addDepth(qreal depth);
    void setDepth(qreal depth);
    void setMaterial(Material* material);
    qreal getDepth();

    qreal isHit(QVector3D vector, qreal max);

    void drawBoundingBox();

protected:
    bool _initialized;
    virtual void _initialize();

    QMatrix4x4* _transformation;

    GLenum dataType;

    // Old stuff here...
    qreal _scale;
    QVector3D _position;
    qreal _rotationX;
    qreal _rotationY;
    qreal _rotationZ;
    // ..

    QVector<Material*> _materials;

    void rotate();
    void move();

    QVector3D _boundingBoxMax;
    QVector3D _boundingBoxMin;

    QVector<GLfloat> _faces;

    bool _bufferEnabled;
    QGLBuffer* _vertexBuffer;
signals:
    void redraw();
    void rotationChanged();
    void positionChanged();
    void scaleChanged();
public slots:
    virtual void clicked();
    virtual void animate();

};

#endif // QLOBJECT_H
