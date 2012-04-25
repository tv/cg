#include "camera.h"

Camera::Camera(QObject *parent) :
    QObject(parent)
{
    this->position  = QVector4D(0.0, 0.0, 0.0, 1.0);
    this->direction = QVector3D(0.0, 0.0, -1.0),
    this->up        = QVector3D(0.0, 1.0, 0.0);

    this->beta = 0.0f;
    this->theta = 0.0f;

    this->calculateDirection();

    this->projection = QMatrix4x4();
    this->projection.perspective(45.0f, 1.0f, 1.0f, 100.0f);
}

void Camera::setPerspective(qreal fov, qreal aspect, qreal near, qreal far) {
    this->projection.setToIdentity();
    this->projection.perspective(fov, aspect, near, far);
}


void Camera::strafeLeft() {
    this->position += QVector3D(-0.2, 0.0, 0.0);
}

void Camera::strafeRight() {
    this->position += QVector3D(0.2, 0.0, 0.0);
}

void Camera::moveBack(){
    this->position -= this->direction / 2;
}

void Camera::moveForward() {
    this->position += this->direction / 3;
}

void Camera::moveUp() {
    this->position += QVector3D(0.0, 0.2, 0.0);
}

void Camera::moveDown() {
    this->position += QVector3D(0.0, -0.2, 0.0);
}

void Camera::rotateLeft() {
    this->beta += 1;
    this->calculateDirection();
}

void Camera::rotateRight() {
    this->beta -= 1;
    this->calculateDirection();
}

void Camera::rotateUp() {
    this->theta += 1;
    this->calculateDirection();
}

void Camera::rotateDown() {
    this->theta -= 1;
    this->calculateDirection();
}

void Camera::calculateDirection() {

    QQuaternion b = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0),this->beta),
                t = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0),this->theta),
                d = b*t;

    d.normalize();



    this->direction = d.rotatedVector(QVector3D(0,0,-1));

    qDebug() << this->direction << this->beta << this->theta;
}

void Camera::injectToShader(QGLShaderProgram *p, QString prefix) {
    QMatrix4x4 mat = this->getMatrix();
    int loc;

    // view
    QString name = QString("%1_v").arg(prefix);
    loc = p->uniformLocation(name);
    if (loc == -1) {
        qDebug() << name << "failed";
    }
    p->setUniformValue(loc, mat);

    // perspective
    name = QString("%1_p").arg(prefix);
    loc = p->uniformLocation(name);
    if (loc == -1) {
        qDebug() << name << "failed";
    }
    p->setUniformValue(loc, this->projection);

    // inverse view
    name = QString("%1_v_inv").arg(prefix);
    loc = p->uniformLocation(name);
    p->setUniformValue(loc, mat.inverted());


}
void Camera::injectToShader(QGLShaderProgram *p) {

    QMatrix4x4 mat = this->getMatrix();
    int loc;

    // view
    loc = p->uniformLocation("v");
    if (loc == -1) {
        qDebug() << "v failed";
    }
    p->setUniformValue(loc, mat);

    // perspective
    loc = p->uniformLocation("p");
    if (loc == -1) {
        qDebug() << "p failed";
    }
    p->setUniformValue(loc, this->projection);

    // inverse view
    loc = p->uniformLocation("v_inv");
    p->setUniformValue(loc, mat.inverted());
}

QMatrix4x4 Camera::getMatrix() {

    QMatrix4x4 mat = QMatrix4x4();

    mat.lookAt(
        this->position.toVector3D(),
        this->position.toVector3D()+this->direction,
        this->up
    );

    return mat;
}
