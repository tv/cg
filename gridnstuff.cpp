#include "gridnstuff.h"

GridNStuff::GridNStuff(QObject *parent) :
    QLObject(parent)
{
    this->dataType = GL_QUADS;
}


void GridNStuff::drawPixel(int xin, int yin, int zin, float color)
{
    GLfloat size = 100;
    QVector<QVector4D> verts;

    GLfloat x = xin, y = yin, z = zin;

    GLfloat v[8][3] = {
        // front
        {0.0, 0.0,  1.0},
        { 1.0, 0.0,  1.0},
        { 1.0,  1.0,  1.0},
        {0.0,  1.0,  1.0},
        // back
        {0.0, 0.0, 0.0},
        { 1.0, 0.0, 0.0},
        { 1.0,  1.0, 0.0},
        {0.0,  1.0, 0.0},
    };

    GLushort e[] = {
        // front
        0, 1, 2, 3,
        // top
        1, 5, 6, 2,
        // back
        7, 6, 5, 4,
        // bottom
        4, 0, 3, 7,
        // left
        4, 5, 1, 0,
        // right
        3, 2, 6, 7
    };


    int j = 0;
    for (j = 0; j < 6; j++) {

        int i;
        for (i = 0; i < 4; i++) {
            GLfloat* ver = v[ e[j*4+i] ];
            verts.append(
                QVector4D(
                    (x + ver[0]) / size,
                    (y + ver[1]) / size,
                    (z + ver[2]) / size,
                    1.0f
                )
            );
        }

        QVector3D norm = QVector3D::normal(
            verts.at(0).toVector3D(),
            verts.at(1).toVector3D(),
            verts.at(2).toVector3D()
        );

        for (i = 0; i < 4; i++) {
            this->_faces.append(verts.at(i).x());
            this->_faces.append(verts.at(i).y());
            this->_faces.append(verts.at(i).z());
            this->_faces.append(verts.at(i).w());

            this->_faces.append(norm.x());
            this->_faces.append(norm.y());
            this->_faces.append(norm.z());

            this->_faces.append(color);
        }

        verts.clear();
    }
}

void GridNStuff::drawPixel(int xin, int yin)
{
    this->drawPixel(xin, yin, 0, 1.0f);
}

void GridNStuff::drawPixel(int xin, int yin, int zin)
{
    this->drawPixel(xin, yin, zin, 1.0f);
}

void GridNStuff::drawPixel(int xin, int yin, float color)
{
    this->drawPixel(xin, yin, 0, color);
}

void GridNStuff::gridPlane()
{
    this->drawPixel(0,0,0, 0.5);


    int i = 0, j = 0;
    while (i <= 100) {
        while (j <= 100) {
            this->drawPixel( i, j, 0, 0.5);
            this->drawPixel(-i, j, 0, 0.5);
            this->drawPixel(-i,-j, 0, 0.5);
            this->drawPixel( i,-j, 0, 0.5);

            j += 10;
        }
        i += 10;
    }

}

void GridNStuff::lineDDA(QVector2D start, QVector2D end)
{
    this->lineDDA(QVector3D(start, 0), QVector3D(end, 0));
}

void GridNStuff::lineDDA(QVector3D start, QVector3D end)
{
    int dx = end.x() - start.x(),
        dy = end.y() - start.y(),
        dz = end.z() - start.z();

    int steps;
    if (abs(dx) >= abs(dy) && abs(dx) >= abs(dz)) {
        // delta x greatest

        steps = abs(dx);
    } else if(abs(dy) >= abs(dx) && abs(dy) >= abs(dz)) {
        steps = abs(dy);
    } else {
        steps = abs(dz);
    }

    float xinc = (float)dx / (float)steps,
          yinc = (float)dy / (float)steps,
          zinc = (float)dz / (float)steps;


    float x = start.x(),
          y = start.y(),
          z = start.z();

    this->drawPixel((int)round(x),(int)round(y),(int)round(z));

    for (int i = 0; i < steps; i++) {
        x += xinc;
        y += yinc;
        z += zinc;

        this->drawPixel((int)round(x),(int)round(y),(int)round(z));
    }
}

void GridNStuff::circleMid(QVector2D mid, int radius)
{

    int f = 1 - radius;
    int ddF_x = 1;
    int ddF_y = -2 * radius;

    int x = 0;
    int y = radius;

    this->drawPixel(mid.x(), mid.y() + radius);
    this->drawPixel(mid.x(), mid.y() - radius);
    this->drawPixel(mid.x() + radius, mid.y());
    this->drawPixel(mid.x() - radius, mid.y());

    while (x < y) {
        if ( f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }

        x++;
        ddF_x += 2;
        f += ddF_x;

        this->drawPixel(mid.x() + x, mid.y() + y);
        this->drawPixel(mid.x() - x, mid.y() + y);
        this->drawPixel(mid.x() + x, mid.y() - y);
        this->drawPixel(mid.x() - x, mid.y() - y);

        this->drawPixel(mid.x() + y, mid.y() + x);
        this->drawPixel(mid.x() - y, mid.y() + x);
        this->drawPixel(mid.x() + y, mid.y() - x);
        this->drawPixel(mid.x() - y, mid.y() - x);
    }
}