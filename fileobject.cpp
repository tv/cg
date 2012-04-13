#include "fileobject.h"
#define BUFFER_OFFSET(i) (reinterpret_cast<void*>(i))
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
    QList<QVector3D*> vertexTemp, normTemp;

    do {
        line = stream.readLine();

        if (line.startsWith("vn")) {
            QStringList list = line.split(' ');


            QVector3D* vector = new QVector3D(
                list.at(1).toFloat(),
                list.at(2).toFloat(),
                list.at(3).toFloat()
            );

            normTemp.append(vector);
        } else if (line.startsWith('v')) {
            QStringList list = line.split(' ');

            QVector3D* vector = new QVector3D(
                list.at(1).toFloat(),
                list.at(2).toFloat(),
                list.at(3).toFloat()
            );

            qreal _x = vector->x();
            qreal _y = vector->y();
            qreal _z = vector->z();

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

            vertexTemp.append(vector);
        } else if (line.startsWith('f')) {
            QStringList list = line.split(' ');

            for (int i = 1; i < list.size(); i++) {
                int vert = list.at(i).section('/', 0, 0).toInt();
                int normal = list.at(i).section('/', 2, 2).toInt();


                this->_vertexes.append(vertexTemp.at(vert-1));
                if (normal != 0) {
                    this->_vertexNorms.insert(this->_vertexes.size(), normTemp.at(normal-1));
                }
            }
        }
    } while (!line.isNull());

    this->_vertexBuffer = new QGLBuffer(QGLBuffer::VertexBuffer);

}

void FileObject::setMode(int mode) {
    this->_mode = mode;
}

void FileObject::draw()
{
    glPushMatrix();

    this->_material.inject();

    this->move();
    this->rotate();

    this->drawBoundingBox();


    if (this->_vertexes.size() > 0) {
        if (!this->_vertexBuffer->isCreated()) {

            int size = this->_vertexes.size();

            this->_vertexBuffer->create();

            this->_vertexBuffer->bind();
            this->_vertexBuffer->setUsagePattern(QGLBuffer::DynamicDraw);
            this->_vertexBuffer->allocate(2 * 3 * size * sizeof(GLfloat));

            GLfloat* qs = (GLfloat*)this->_vertexBuffer->map(QGLBuffer::ReadWrite);

            QVector3D normal = QVector3D::normal(
                *this->_vertexes.at(0),
                *this->_vertexes.at(1),
                *this->_vertexes.at(2)
            );

            QVector3D* n;

            for (int i = 0; i < size; i++) {
                if ((i+1)%3 == 0) {
                    normal = QVector3D::normal(
                        *this->_vertexes.at(i-2),
                        *this->_vertexes.at(i-1),
                        *this->_vertexes.at(i)
                    );
                }
                qs[i*6+0] = this->_vertexes.at(i)->x();
                qs[i*6+1] = this->_vertexes.at(i)->y();
                qs[i*6+2] = this->_vertexes.at(i)->z();

                n = this->_vertexNorms.value(i);


                if (n->isNull()) {
                    qs[i*6+3] = normal.x();
                    qs[i*6+4] = normal.y();
                    qs[i*6+5] = normal.z();
                } else {
                    qs[i*6+3] = n->x();
                    qs[i*6+4] = n->y();
                    qs[i*6+5] = n->z();
                }
            }

            this->_vertexBuffer->unmap();
        }

        this->_vertexBuffer->bind();

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glVertexPointer(3, GL_FLOAT, 6*sizeof(GLfloat), BUFFER_OFFSET(0));
        glNormalPointer(GL_FLOAT, 6*sizeof(GLfloat), BUFFER_OFFSET(3*sizeof(GLfloat)));

        glDrawArrays(GL_TRIANGLES, 0, this->_vertexes.size());

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);

        this->_vertexBuffer->release();
    }


    glPopMatrix();
}
