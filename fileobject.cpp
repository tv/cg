#include "fileobject.h"
#define BUFFER_OFFSET(i) (reinterpret_cast<void*>(i))
FileObject::FileObject(QObject *parent) :
    QLObject(parent)
{
    this->ignoreClickEvent = false;

    this->_initialized = false;

    this->_vertexBuffer = new QGLBuffer(QGLBuffer::VertexBuffer);
}

void FileObject::readFile(QString path) {
    QFile myFile(path);

    QString line;

    if (!myFile.open(QIODevice::ReadOnly)) {
        std::cout << "Error reading file" << std::endl;
        return;
    }

    QTextStream stream(&myFile);
    QList<QVector4D*> vertexTemp;
    QList<QVector3D*> normTemp;

    QString material;

    QList<QString> mtlTemp;

    GLfloat materialId = 0;

    do {
        line = stream.readLine();
        QStringList list = line.split(' ');

        if (line.startsWith("vn")) {
            QVector3D* vector = new QVector3D(
                list.at(1).toFloat(),
                list.at(2).toFloat(),
                list.at(3).toFloat()
            );

            normTemp.append(vector);
        } else if (line.startsWith("mtllib")) {
            mtlTemp.append(list.at(1));

        } else if (line.startsWith('v')) {
            QVector4D* vector = new QVector4D(
                list.at(1).toFloat(),
                list.at(2).toFloat(),
                list.at(3).toFloat(),
                1.0f
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


            QVector4D* vec;
            QVector3D* norm;
            int i;


            // check normals

            bool noNormals = true;
            for (i = 1; i < list.size(); i++) {
                if (list.at(i).section('/', 2, 2).toInt() != 0) {
                    noNormals = false;
                }
            }

            if (noNormals) {

                int verts[] = {
                    list.at(1).section('/', 0, 0).toInt()-1,
                    list.at(2).section('/', 0, 0).toInt()-1,
                    list.at(3).section('/', 0, 0).toInt()-1
                };
                QVector3D normal = QVector3D::normal(
                    vertexTemp.at(verts[0])->toVector3D(),
                    vertexTemp.at(verts[1])->toVector3D(),
                    vertexTemp.at(verts[2])->toVector3D()
                );

                for (i = 1; i < list.size(); i++) {
                    vec = vertexTemp.at(list.at(i).section('/', 0, 0).toInt() - 1);
                    this->_faces.append(vec->x());
                    this->_faces.append(vec->y());
                    this->_faces.append(vec->z());
                    this->_faces.append(vec->w());

                    this->_faces.append(normal.x());
                    this->_faces.append(normal.y());
                    this->_faces.append(normal.z());

                    this->_faces.append(materialId); // Material id
                }
            } else {
                for (i = 1; i < list.size(); i++) {
                    vec = vertexTemp.at(list.at(i).section('/', 0, 0).toInt() - 1);
                    norm = normTemp.at(list.at(i).section('/', 2, 2).toInt() - 1);

                    this->_faces.append(vec->x());
                    this->_faces.append(vec->y());
                    this->_faces.append(vec->z());
                    this->_faces.append(vec->w());

                    this->_faces.append(norm->x());
                    this->_faces.append(norm->y());
                    this->_faces.append(norm->z());

                    this->_faces.append(materialId); // Material id
                }
            }

        } else if (line.startsWith("usemtl")) {
            material = list.at(1);
        }
    } while (!line.isNull());

    for (int i = 0; i < mtlTemp.size(); i++) {

        QFile myFile(mtlTemp.at(i));

        QString line;

        if (!myFile.open(QIODevice::ReadOnly)) {
            std::cout << "Error reading file" << std::endl;
            return;
        }

        QTextStream mtlStream(&myFile);

        Material* mat;
        do {
            line = mtlStream.readLine();
            QStringList list = line.split(' ');

            if (line.startsWith("newmtl")) {
                mat = new Material();
                qDebug() << list.at(1);
                this->_materials.append(mat);

            } else if (line.startsWith("Ka")) {

                mat->setAmbient(QVector4D(
                    list.at(1).toFloat(),
                    list.at(2).toFloat(),
                    list.at(3).toFloat(),
                    1.0f
                ));

            } else if (line.startsWith("Kd")) {
                mat->setDiffuse(QVector4D(
                    list.at(1).toFloat(),
                    list.at(2).toFloat(),
                    list.at(3).toFloat(),
                    1.0f
                ));
            } else if (line.startsWith("Ks")) {
                mat->setSpecular(QVector4D(
                    list.at(1).toFloat(),
                    list.at(2).toFloat(),
                    list.at(3).toFloat(),
                    1.0f
                ));
            } else if (line.startsWith("Ns")) {
                mat->setShihiness(list.at(1).toFloat());
            }


        } while (!line.isNull());
    }



}

void FileObject::fillBuffer(QGLBuffer *b, QList<QVector4D*> data)
{
    int size = data.size();

    int allocBits = 4 * size * sizeof(GLfloat);
    GLfloat* qs = (GLfloat*) malloc(allocBits);


    for (int i = 0; i < size; i++) {
        qs[i*4+0] = data.at(i)->x();
        qs[i*4+1] = data.at(i)->y();
        qs[i*4+2] = data.at(i)->z();
        qs[i*4+3] = data.at(i)->w();
    }


    b->create();
    b->bind();
    b->allocate(qs, allocBits);
    b->setUsagePattern(QGLBuffer::StaticDraw);
    b->release();

    free(qs);

    data.clear();
}

void FileObject::fillBuffer(QGLBuffer *b, QList<GLushort> data)
{
    int size = data.size();

    int allocBits = size * sizeof(GLushort);

    GLushort* qs = (GLushort*) malloc(allocBits);

    for (int i = 0; i < size; i++) {
        qs[i] = data.at(i);
    }

    b->create();
    b->bind();
    b->allocate(qs, allocBits);
    b->setUsagePattern(QGLBuffer::StaticDraw);
    b->release();

    free(qs);

    data.clear();
}


void FileObject::_initialize()
{
    if (this->_faces.size() > 0) {

        int size = this->_faces.size();

        int allocBits = size * sizeof(GLfloat);

        this->_vertexBuffer->create();
        this->_vertexBuffer->bind();
        this->_vertexBuffer->allocate(this->_faces.data(), allocBits);
        this->_vertexBuffer->setUsagePattern(QGLBuffer::StaticDraw);
        this->_vertexBuffer->release();

        this->_faces.clear();

    }

    this->_initialized = true;
}
