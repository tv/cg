#ifndef LIGHT_H
#define LIGHT_H

#include <QObject>
#include <QVector3D>
#include <QVector4D>
#include <QtOpenGL>


class Light : public QObject
{
Q_OBJECT
public:
    explicit Light(QObject *parent = 0);

    void outputFormat(QGLShaderProgram *p, QString name);

    QVector4D position;
    QVector4D diffuse;
    QVector4D specular;

    QVector3D attenuations;

    QVector3D direction;

    float spotCutOff, spotExponent;
protected:
    qreal angle;
signals:

    void redraw();
public slots:
    void animate();

};

#endif // LIGHT_H

