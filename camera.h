#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <QtOpenGL>
class Camera : public QObject
{
    Q_OBJECT
public:
    explicit Camera(QObject *parent = 0);
    
    QVector3D position;
    QVector3D direction;
    QVector3D up;
    QMatrix4x4 projection;

    void setPerspective(qreal fov, qreal aspect, qreal near, qreal far);

    void strafeLeft();
    void strafeRight();

    void moveBack();
    void moveForward();
    void moveUp();
    void moveDown();

    void rotateLeft();
    void rotateRight();
    void rotateUp();
    void rotateDown();


    float beta;
    float theta;


    QMatrix4x4 getMatrix();

    void injectToShader(QGLShaderProgram *p);

    void injectToShader(QGLShaderProgram *p, QString prefix);
signals:
    
public slots:

protected:
    void calculateDirection();
};

#endif // CAMERA_H
