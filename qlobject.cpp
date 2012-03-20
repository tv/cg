#include "qlobject.h"

QLObject::QLObject(QObject *parent) :
    QObject(parent)
{
    this->_position = QVector3D(0, 0, -10);

    this->_rotationX = 0;
    this->_rotationY = 0;
    this->_rotationZ = 0;

    this->_scale = 1.0;
}

void QLObject::setPosition(const QVector3D& value)
{
    this->_position = value;
    emit positionChanged();
    emit redraw();
}

void QLObject::setPosition(const qreal x, const qreal y)
{
    this->_position.setX(x);
    this->_position.setY(y);

    emit positionChanged();
    emit redraw();
}

void QLObject::setScale(qreal value)
{
    this->_scale = value;
    emit scaleChanged();
    emit redraw();
}

void QLObject::setRotateX(qreal angle)
{
    this->_rotationX = angle;
    emit rotationChanged();
    emit redraw();
}

void QLObject::setRotateY(qreal angle)
{
    this->_rotationY = angle;
    emit rotationChanged();
    emit redraw();
}

void QLObject::setRotateZ(qreal angle)
{
    this->_rotationZ = angle;
    emit rotationChanged();
    emit redraw();
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
    glRotatef(this->_rotationX, 1, 0, 0);
    glRotatef(this->_rotationY, 0, 1, 0);
    glRotatef(this->_rotationZ, 0, 0, 1);
}

void QLObject::move()
{
    glTranslatef(
        this->_position.x(),
        this->_position.y(),
        this->_position.z()
    );
}

void QLObject::addRotateX(qreal angle)
{
    qreal rotation = this->_rotationX;

    rotation += angle;

    this->setRotateX(rotation);
}

void QLObject::addRotateY(qreal angle)
{
    qreal rotation = this->_rotationY;

    rotation += angle;

    this->setRotateY(rotation);
}

void QLObject::addRotateZ(qreal angle)
{
    qreal rotation = this->_rotationZ;

    rotation += angle;

    this->setRotateZ(rotation);
}

qreal QLObject::getDepth()
{
    return this->_position.z();
}


void QLObject::clicked()
{
    std::cout << "Clicked!" << std::endl;
}
