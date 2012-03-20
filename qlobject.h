#ifndef QLOBJECT_H
#define QLOBJECT_H

#include <QObject>
#include <QVector3D>
#include <QtOpenGL>

struct rotation {
    qreal angle;
    qreal x;
    qreal y;
    qreal z;
};

class QLObject : public QObject
{
Q_OBJECT
public:
    explicit QLObject(QObject *parent = 0);
    void setPosition(const QVector3D&);
    void setScale(qreal value);
    void setRotate(qreal angle, qreal x, qreal y, qreal z);

    virtual void draw();
    virtual void initialize();

protected:
    QVector3D _position;
    qreal _scale;
    rotation _rotation;

    void vertex(qreal x, qreal y);
    void vertex(qreal x, qreal y, qreal z);
    void color(qreal r, qreal g, qreal b);

    void rotate();
    void move();
signals:
    void positionChanged();
    void scaleChanged();
public slots:

};

#endif // QLOBJECT_H
