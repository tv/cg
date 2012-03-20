#ifndef QLOBJECT_H
#define QLOBJECT_H

#include <QObject>
#include <QVector3D>
#include <QtOpenGL>
#include <iostream>

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

    void addRotateX(qreal angle);
    void addRotateY(qreal angle);
    void addRotateZ(qreal angle);

    virtual void draw();
    virtual void initialize();

    qreal getDepth();

protected:
    QVector3D _position;
    qreal _scale;

    qreal _rotationX;
    qreal _rotationY;
    qreal _rotationZ;

    void vertex(qreal x, qreal y);
    void vertex(qreal x, qreal y, qreal z);
    void color(qreal r, qreal g, qreal b);

    void rotate();
    void move();
signals:
    void redraw();
    void rotationChanged();
    void positionChanged();
    void scaleChanged();
public slots:
    virtual void clicked();

};

#endif // QLOBJECT_H
