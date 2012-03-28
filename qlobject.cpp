#include "qlobject.h"

QLObject::QLObject(QObject *parent) :
    QObject(parent)
{
    this->_position = QVector3D(0, 0, -10);

    this->_rotationX = 0;
    this->_rotationY = 0;
    this->_rotationZ = 0;

    this->_scale = 1.0;

    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
    GLfloat mat_ambient_color[] = { 0.8, 0.8, 0.2, 1.0 };
    GLfloat mat_diffuse[] = { 0.1, 0.5, 0.8, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat no_shininess[] = { 0.0 };
    GLfloat low_shininess[] = { 5.0 };
    GLfloat high_shininess[] = { 100.0 };
    GLfloat mat_emission[] = {0.3, 0.2, 0.2, 0.0};
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

void QLObject::setDepth(qreal depth)
{
    this->_position.setZ(depth);

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

void QLObject::vertex(QVector3D v1)
{
    this->vertex(v1.x(), v1.y(), v1.z());
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

void QLObject::surface(QVector3D v1, QVector3D v2, QVector3D v3)
{
    QVector3D normal = QVector3D::normal(v1, v2, v3);

    glNormal3f(normal.x(), normal.y(), normal.z());
    this->vertex(v1);
    this->vertex(v2);
    this->vertex(v3);
}

void QLObject::surface(QVector3D v1, QVector3D v2, QVector3D v3, QVector3D v4)
{
    QVector3D normal = QVector3D::normal(v1, v2, v3);

    glNormal3f(normal.x(), normal.y(), normal.z());
    this->vertex(v1);
    this->vertex(v2);
    this->vertex(v3);
    this->vertex(v4);
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

void QLObject::addRotate(qreal x, qreal y, qreal z)
{
    this->_rotationX += x;
    this->_rotationY += y;
    this->_rotationZ += z;

    emit redraw();
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

void QLObject::addDepth(qreal depth)
{
    qreal cur = this->_position.z();
    cur += depth;
    this->setDepth(cur);
}

qreal QLObject::getDepth()
{
    return this->_position.z();
}


void QLObject::clicked()
{
    std::cout << "Clicked!" << std::endl;
}
