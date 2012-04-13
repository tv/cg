#ifndef FILEOBJECT_H
#define FILEOBJECT_H

#include "qlobject.h"
#include <QTextStream>

class FileObject : public QLObject
{
Q_OBJECT
public:
    explicit FileObject(QObject *parent = 0);

    void readFile(QString path);

    void draw();
    void setMode(int mode);

protected:
    QList<QVector3D*> _vertexes;
    QList<QVector3D*> _vertexNorms;

    QGLBuffer* _vertexBuffer;

    int _mode;
signals:

public slots:

};

#endif // FILEOBJECT_H
