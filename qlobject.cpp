#include "qlobject.h"

QLObject::QLObject(QObject *parent) :
    QObject(parent)
{
    this->_position = QVector3D(0, 0, -10);

    this->_rotation.angle = 0;
    this->_rotation.x = 0;
    this->_rotation.y = 0;
    this->_rotation.z = 0;

    this->_scale = 1.0;
}

void QLObject::setPosition(const QVector3D& value)
{
    this->_position = value;
    emit positionChanged();
}

void QLObject::setScale(qreal value)
{
    this->_scale = value;
    emit scaleChanged();
}

void QLObject::setRotate(qreal angle, qreal x, qreal y, qreal z)
{
    this->_rotation.angle = angle;
    this->_rotation.x = x;
    this->_rotation.y = y;
    this->_rotation.z = z;
}

void QLObject::draw()
{

}
void QLObject::initialize()
{
}

void QLObject::vertex(qreal x, qreal y)
{
    qreal _x = x * this->_scale;
    qreal _y = y * this->_scale;
    glVertex2f(_x, _y);
}

void QLObject::vertex(qreal x, qreal y, qreal z)
{
    qreal _x = x * this->_scale;
    qreal _y = y * this->_scale;
    qreal _z = z * this->_scale;
    glVertex3f(_x, _y, _z);
}

void QLObject::color(qreal r, qreal g, qreal b)
{
    glColor3f(r,g,b);
}

void QLObject::rotate()
{
    glRotatef(
        this->_rotation.angle,
        this->_rotation.x,
        this->_rotation.y,
        this->_rotation.z
    );
}

void QLObject::move()
{
    glTranslatef(
        this->_position.x(),
        this->_position.y(),
        this->_position.z()
    );
}
