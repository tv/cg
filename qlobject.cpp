#include "qlobject.h"

QLObject::QLObject(QObject *parent) :
    QObject(parent)
{
    this->_position = QVector3D(0, 0, -10);

    this->_rotationX = 0;
    this->_rotationY = 0;
    this->_rotationZ = 0;

    this->_scale = 1.0;

    this->_boundingBoxMin = QVector3D(0,0,0);
    this->_boundingBoxMax = QVector3D(0,0,0);

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

void QLObject::setMaterial(Material material)
{
    this->_material = material;
    emit redraw();
}

/**
 * return the depth of hit
 * return -100.0f if not hit
 */
qreal QLObject::isHit(QVector3D position, qreal loopMax)
{
    QMatrix4x4 m;

    m.translate(this->_position);
    m.rotate(this->_rotationX, 1.0f, 0.0f, 0.0f);
    m.rotate(this->_rotationY, 0.0f, 1.0f, 0.0f);
    m.rotate(this->_rotationZ, 0.0f, 0.0f, 1.0f);

    QVector3D mapped1 = m.map(this->_boundingBoxMax),
              mapped2 = m.map(this->_boundingBoxMin),
              point;


    qreal maxX = mapped1.x(),
          maxY = mapped1.y(),
          maxZ = mapped1.z(),
          minX = mapped2.x(),
          minY = mapped2.y(),
          minZ = mapped2.z();

    if (minX > maxX) {
        minX = maxX;
        maxX = mapped2.x();
    }

    if (minY > maxY) {
        minY = maxY;
        maxY = mapped2.y();
    }

    if (minZ > maxZ) {
        minZ = maxZ;
        maxZ = mapped2.z();
    }
    QVector3D max = QVector3D(maxX, maxY, maxZ);
    QVector3D min = QVector3D(minX, minY, minZ);

    qreal iter = loopMax,
          i;

    for (i = 1.0f; i < iter; i++) {
        point = position * i;

        if (point.x() > min.x() && point.y() > min.y() && point.z() > min.z()
            && point.x() < max.x() && point.y() < max.y() && point.z() < max.z()) {

            break;
        }
    }
    return i;
}

void QLObject::drawBoundingBox()
{
    qreal maX = this->_boundingBoxMin.x(),
          maY = this->_boundingBoxMin.y(),
          maZ = this->_boundingBoxMin.z(),
          miX = this->_boundingBoxMax.x(),
          miY = this->_boundingBoxMax.y(),
          miZ = this->_boundingBoxMax.z();

    glPointSize(3.0f);
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    glBegin(GL_POINTS);
    glVertex3f(maX, maY, maZ);
    glVertex3f(maX, maY, miZ);
    glVertex3f(maX, miY, miZ);
    glVertex3f(maX, miY, maZ);
    glVertex3f(miX, maY, maZ);
    glVertex3f(miX, maY, miZ);
    glVertex3f(miX, miY, miZ);
    glVertex3f(miX, miY, maZ);
    glEnd();
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

    qreal _maxBBx = this->_boundingBoxMax.x();
    qreal _maxBBy = this->_boundingBoxMax.y();
    qreal _maxBBz = this->_boundingBoxMax.z();

    qreal _minBBx = this->_boundingBoxMin.x();
    qreal _minBBy = this->_boundingBoxMin.y();
    qreal _minBBz = this->_boundingBoxMin.z();

    if (_x < _minBBx) this->_boundingBoxMin.setX(_x);
    if (_y < _minBBy) this->_boundingBoxMin.setY(_y);
    if (_z < _minBBz) this->_boundingBoxMin.setZ(_z);

    if (_x > _maxBBx) this->_boundingBoxMax.setX(_x);
    if (_y > _maxBBy) this->_boundingBoxMax.setY(_y);
    if (_z > _maxBBz) this->_boundingBoxMax.setZ(_z);

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
