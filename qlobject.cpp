#include "qlobject.h"

#define BUFFER_OFFSET(i) (reinterpret_cast<void*>(i))

QLObject::QLObject(QObject *parent) :
    QObject(parent)
{
    this->_initialized = false;
    this->_boundingBoxMin = QVector3D(0,0,0);
    this->_boundingBoxMax = QVector3D(0,0,0);

    // Old stuff, should be removed some time
    // Matrix mode is not ready yet...
    this->_position = QVector3D(0, 0, -10);
    this->_rotationX = 0;
    this->_rotationY = 0;
    this->_rotationZ = 0;
    this->_scale = 1.0;

    this->dataType = GL_TRIANGLES;
    this->_vertexBuffer = new QGLBuffer(QGLBuffer::VertexBuffer);

    this->_initialized = false;

    this->_bufferEnabled = true;
}

void QLObject::_initialize()
{
    if (this->_bufferEnabled) { // is vertexbuffer enabled
        if (this->_faces.size() > 0) {
            int size = this->_faces.size();

            int allocBits = size * sizeof(GLfloat);


            this->_vertexBuffer->create();
            this->_vertexBuffer->bind();
            this->_vertexBuffer->allocate(this->_faces.data(), allocBits);
            this->_vertexBuffer->setUsagePattern(QGLBuffer::StaticDraw);

            qDebug() << size;

            this->_faces.clear();
        }
    }

    this->_initialized = true;
}

void QLObject::injectToShader(QGLShaderProgram *p)
{
    GLenum errCode;

    if (!this->_initialized) {
        this->_initialize();
    }

    if ((errCode = glGetError()) != GL_NO_ERROR) {

        qDebug() << "QLobject  before inject materials OpenGL Error: " << errCode;
    }

    if (p->uniformLocation("in_material") != -1) {
        this->_materials.at(0)->injectToShader(p);

        if ((errCode = glGetError()) != GL_NO_ERROR) {

            qDebug() << "QLobject before bind  OpenGL Error: " << errCode;

        }
    }

    QMatrix4x4 trans = this->getTransformationMatrix();
    p->setUniformValue("m", trans);
    p->setUniformValue("m_3x3_inv_transp", trans.normalMatrix());

    if ((errCode = glGetError()) != GL_NO_ERROR) {


        qDebug() << "QLobject  after bind  OpenGL Error: " << errCode;
        return;

    }

    p->enableAttributeArray("v_coord");
    p->enableAttributeArray("v_normal");
    p->enableAttributeArray("v_tex");
    p->enableAttributeArray("v_mat");

    if (this->_bufferEnabled) {



        this->_vertexBuffer->bind();

        if ((errCode = glGetError()) != GL_NO_ERROR) {
            qDebug() << "QLobject  after bind vbo OpenGL Error: " << errCode;
        }

        p->setAttributeBuffer(
            "v_coord",
            GL_FLOAT,
            0 * sizeof(GLfloat),
            4,
            10 * sizeof(GLfloat)
        );

        p->setAttributeBuffer(
            "v_normal",
            GL_FLOAT,
            4 * sizeof(GLfloat),
            3,
            10 * sizeof(GLfloat)
        );

        p->setAttributeBuffer(
            "v_tex",
            GL_FLOAT,
            7 * sizeof(GLfloat),
            2,
            10 * sizeof(GLfloat)
        );

        p->setAttributeBuffer(
            "v_mat",
            GL_FLOAT,
            9 * sizeof(GLfloat),
            1,
            10 * sizeof(GLfloat)
        );


        glDrawArrays(this->dataType, 0, this->_vertexBuffer->size() / (sizeof(GLfloat)));


        if ((errCode = glGetError()) != GL_NO_ERROR) {
            qDebug() << "QLobject  after drawArr OpenGL Error: " << errCode;
        }

    } else {
        GLfloat* ptr = this->_faces.data();

        p->setAttributeArray("v_coord", ptr, 4, 8);
        p->setAttributeArray("v_coord", ptr+4*sizeof(GLfloat), 3, 8);
        p->setAttributeArray("v_mat", ptr+7*sizeof(GLfloat), 1, 8);
        glDrawArrays(this->dataType, this->_faces.size(), 1);
    }

    p->disableAttributeArray("v_coord");
    p->disableAttributeArray("v_normal");
    p->disableAttributeArray("v_tex");
    p->disableAttributeArray("v_mat");

    if ((errCode = glGetError()) != GL_NO_ERROR) {
        qDebug() << "QLobject  after draw OpenGL Error: " << errCode;
    }



    if (this->_bufferEnabled) {
        this->_vertexBuffer->release();
    }

}

void QLObject::setPosition(const QVector3D& value)
{
    this->_position = value;
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

void QLObject::setMaterial(Material* material)
{
    this->_materials.append(material);
    emit redraw();
}

QMatrix4x4 QLObject::getTransformationMatrix()
{
    QMatrix4x4 m = QMatrix4x4();

    m.setToIdentity();

    m.translate(this->_position);
    m.rotate(this->_rotationX, 1.0f, 0.0f, 0.0f);
    m.rotate(this->_rotationY, 0.0f, 1.0f, 0.0f);
    m.rotate(this->_rotationZ, 0.0f, 0.0f, 1.0f);
    m.scale(this->_scale);
    return m;
}

/**
 * return the depth of hit
 * return -100.0f if not hit
 */
qreal QLObject::isHit(QVector3D vector, qreal loopMax)
{
    QMatrix4x4 m = this->getTransformationMatrix();

    QVector3D mapped1 = m * this->_boundingBoxMax,
              mapped2 = m * this->_boundingBoxMin,
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
        point = vector * i;

        if (min.x() < point.x() && min.y() < point.y() && min.z() < point.z() &&
            point.x() < max.x() && point.y() < max.y() && point.z() < max.z()
        ) {
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

void QLObject::animate()
{
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
