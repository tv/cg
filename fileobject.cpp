#include "fileobject.h"

FileObject::FileObject(QObject *parent) :
    QLObject(parent)
{
    this->ignoreClickEvent = false;
    this->_mode = 2;
}

void FileObject::readFile(QString path) {
    QFile myFile(path);

    QString line;

    if (!myFile.open(QIODevice::ReadOnly)) {
        std::cout << "Error reading file" << std::endl;
        return;
    }

    QTextStream stream(&myFile);
    QList<QVector3D*> vertexTemp;
    do {
        line = stream.readLine();

        if (line.startsWith('v')) {
            QStringList list = line.split(' ');

            QVector3D* vector = new QVector3D(
                list.at(1).toFloat(),
                list.at(2).toFloat(),
                list.at(3).toFloat()
            );

            vertexTemp.append(vector);
        } else if (line.startsWith('f')) {
            QStringList list = line.split(' ');

            if (list.size() == 4) {
                qDebug() << list.at(1) << list.at(2) << list.at(3);
                this->_vertexes.append(vertexTemp.at(list.at(1).toInt()-1));
                this->_vertexes.append(vertexTemp.at(list.at(2).toInt()-1));
                this->_vertexes.append(vertexTemp.at(list.at(3).toInt()-1));
            } else if (list.size() == 5) {
                this->_quads.append(vertexTemp.at(list.at(1).toInt()-1));
                this->_quads.append(vertexTemp.at(list.at(2).toInt()-1));
                this->_quads.append(vertexTemp.at(list.at(3).toInt()-1));
                this->_quads.append(vertexTemp.at(list.at(4).toInt()-1));
            }
        }
    } while (!line.isNull());

}

void FileObject::setMode(int mode) {
    this->_mode = mode;
}

void FileObject::draw()
{
    glPushMatrix();
    //glColor4f(1.0, 0.75, 0.75, 0.25);


    this->_material.inject();

    this->move();
    this->rotate();

    this->drawBoundingBox();
    int i, length;

    length = this->_quads.size();
    i = 0;
    while (i < length) {
        QVector3D* v1 = this->_quads.at(i);
        QVector3D* v2 = this->_quads.at(i+1);
        QVector3D* v3 = this->_quads.at(i+2);
        QVector3D* v4 = this->_quads.at(i+3);
        glBegin(GL_QUADS);
        this->surface(*v1,*v2,*v3, *v4);
        glEnd();
        i += 4;
    }

    length = this->_vertexes.size();
    i = 0;
    while (i < length) {
        QVector3D* v1 = this->_vertexes.at(i);
        QVector3D* v2 = this->_vertexes.at(i+1);
        QVector3D* v3 = this->_vertexes.at(i+2);
        glBegin(GL_TRIANGLES);
        this->surface(*v1,*v2,*v3);
        glEnd();
        i += 3;
    }

    glPopMatrix();
}
