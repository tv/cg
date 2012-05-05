#include "fileobject.h"
#define BUFFER_OFFSET(i) (reinterpret_cast<void*>(i))
FileObject::FileObject(QObject *parent) :
    QLObject(parent)
{
    this->ignoreClickEvent = false;
}

int FileObject::stringToInt(QString str, bool *ok)
{
    if (!ok) {
        return 0;
    }
    return str.toInt(ok);
}

float FileObject::stringToFloat(QString str, bool *ok)
{
    if (!ok) {
        return 0.0;
    }
    return str.toFloat(ok);
}

void FileObject::readFile(QString path) {
    QString dir = "objects/";
    QFile myFile(dir+path);

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
        bool ok = true;
        line = stream.readLine();
        QStringList list = line.split(' ');

        if (line.startsWith("vn")) {
            QVector3D* vector = new QVector3D(
                stringToFloat(list.at(1), &ok),
                stringToFloat(list.at(2), &ok),
                stringToFloat(list.at(3), &ok)
            );

            normTemp.append(vector);
        } else if (line.startsWith("mtllib")) {
            mtlTemp.append(list.at(1));

        } else if (line.startsWith('v')) {
            QVector4D* vector = new QVector4D(
                stringToFloat(list.at(1), &ok),
                stringToFloat(list.at(2), &ok),
                stringToFloat(list.at(3), &ok),
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
                if (stringToInt(list.at(i).section('/', 2, 2), &ok) != 0) {
                    noNormals = false;
                }
            }

            if (noNormals) {

                int verts[] = {
                    stringToInt(list.at(1).section('/', 0, 0), &ok)-1,
                    stringToInt(list.at(2).section('/', 0, 0), &ok)-1,
                    stringToInt(list.at(3).section('/', 0, 0), &ok)-1,
                };
                QVector3D normal = QVector3D::normal(
                    vertexTemp.at(verts[0])->toVector3D(),
                    vertexTemp.at(verts[1])->toVector3D(),
                    vertexTemp.at(verts[2])->toVector3D()
                );

                for (i = 1; i < list.size(); i++) {
                    vec = vertexTemp.at(stringToInt(list.at(i).section('/', 0, 0), &ok)-1);
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
                    vec = vertexTemp.at(stringToInt(list.at(i).section('/', 0, 0), &ok)-1);
                    norm = normTemp.at(stringToInt(list.at(i).section('/', 2, 2), &ok)-1);

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

        if (!ok) {
            qDebug() << "PARSER FAILED AT " << line;
        }
    } while (!line.isNull());

    for (int i = 0; i < mtlTemp.size(); i++) {

        QFile myFile(dir+mtlTemp.at(i));

        QString line;

        if (!myFile.open(QIODevice::ReadOnly)) {
            std::cout << "Error reading file" << std::endl;
            return;
        }

        QTextStream mtlStream(&myFile);

        Material* mat;

        do {
            bool ok = true;
            line = mtlStream.readLine();
            QStringList list = line.split(' ');

            if (line.startsWith("newmtl")) {
                mat = new Material();
                qDebug() << list.at(1);
                this->_materials.append(mat);

            } else if (line.startsWith("Ka")) {

                mat->setAmbient(QVector4D(
                    stringToFloat(list.at(1), &ok),
                    stringToFloat(list.at(2), &ok),
                    stringToFloat(list.at(3), &ok),
                    1.0f
                ));

            } else if (line.startsWith("Kd")) {
                mat->setDiffuse(QVector4D(
                    stringToFloat(list.at(1), &ok),
                    stringToFloat(list.at(2), &ok),
                    stringToFloat(list.at(3), &ok),
                    1.0f
                ));
            } else if (line.startsWith("Ks")) {
                mat->setSpecular(QVector4D(
                    stringToFloat(list.at(1), &ok),
                    stringToFloat(list.at(2), &ok),
                    stringToFloat(list.at(3), &ok),
                    1.0f
                ));
            } else if (line.startsWith("Ns")) {
                mat->setShihiness(stringToFloat(list.at(1), &ok));
            }


            if (!ok) {
                qDebug() << "PARSER FAILED AT " << line;
            }
        } while (!line.isNull());
    }
}
