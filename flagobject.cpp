#include "flagobject.h"

FlagObject::FlagObject(QObject *parent) :
    QLObject(parent)
{
}
void FlagObject::draw()
{
    glPushMatrix();
    this->move();
    this->rotate();
    this->_drawBase();
    this->_drawContent();
    glPopMatrix();
}

void FlagObject::_drawBase()
{
    this->color(1,1,1);
    glBegin(GL_TRIANGLES);
    this->vertex(0,    0,    0);
    this->vertex(0,    11.0f, 0);
    this->vertex(18.0f, 0,    0);
    glEnd();

    glBegin(GL_TRIANGLES);
    this->vertex(0,    11.0f, 0);
    this->vertex(18.0f, 11.0f, 0);
    this->vertex(18.0f, 0,    0);
    glEnd();

    this->color(1,0,0);

    glBegin(GL_LINES);
    this->vertex(0,    0,    0);
    this->vertex(0,    11.0f, 0);
    this->vertex(0,    11.0f, 0);
    this->vertex(18.0f, 11.0f, 0);
    this->vertex(18.0f, 11.0f, 0);
    this->vertex(18.0f, 0,    0);
    this->vertex(18.0f, 0,    0);
    this->vertex(0,    0,    0);
    glEnd();
}

void FlagObject::setType(qreal type)
{
    this->_type = type;
}

void FlagObject::_drawContent()
{
    if (this->_type == 0) {
        this->color(0, 0, 1);
        glBegin(GL_TRIANGLES);
        // Vasen ala;

        this->vertex(0, 4.0f, 0);
        this->vertex(18.0f, 4.0f, 0);
        this->vertex(0, 7.0f, 0);
        this->vertex(0, 7.0f, 0);
        this->vertex(18.0f, 7.0f, 0);
        this->vertex(18.0f, 4.0f, 0);

        this->vertex(5.0f, 0, 0);
        this->vertex(8.0f, 0, 0);
        this->vertex(5.0f, 11.0f,0);
        this->vertex(5.0f, 11.0f,0);
        this->vertex(8.0f, 11.0f,0);
        this->vertex(8.0f, 0, 0);


        glEnd();
    } else if (this->_type == 1) {

        this->color(1,0,0);

        glBegin(GL_POLYGON);

        int r = 3;

        for (int i=0; i < 360; i++) {
            float rad = i*3.14159/180;
            this->vertex(9.0f, 5.5f, 0);
            this->vertex(cos(rad)*r+9.0f, sin(rad)*r+5.5f, 0);
            rad = (i+1)*3.14159/180;
            this->vertex(cos(rad)*r+9.0f, sin(rad)*r+5.5f, 0);
        }
        glEnd();
    } else {
        this->color(1,0,0);

        glBegin(GL_LINES);
        this->vertex(0,    0.1f, 0);
        this->vertex(17.9f, 11.0f, 0);
        this->vertex(18.0f, 10.9f, 0);
        this->vertex(0.1f, 0,    0);

        this->vertex(18.0f, 0.1f, 0);
        this->vertex(0.1f, 11.0f, 0);
        this->vertex(0,    10.9f, 0);
        this->vertex(17.9f, 0,    0);
        glEnd();
    }
}
