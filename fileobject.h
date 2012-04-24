#ifndef FILEOBJECT_H
#define FILEOBJECT_H

#include "qlobject.h"
#include <QTextStream>

class FileObject : public QLObject
{
Q_OBJECT
public:
    explicit FileObject(QObject *parent = 0);

    void fillBuffer(QGLBuffer *b, QList<QVector4D*> data);
    void fillBuffer(QGLBuffer *b, QList<GLushort> data);


    void readFile(QString path);

protected:
    QVector<GLfloat> _faces;

    virtual void _initialize();
signals:

public slots:

};

#endif // FILEOBJECT_H
