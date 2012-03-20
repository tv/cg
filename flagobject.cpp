#include "flagobject.h"

FlagObject::FlagObject(QObject *parent) :
    QLObject(parent)
{
    this->ignoreClickEvent = false;
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

    emit redraw();
}

void FlagObject::_drawContent()
{
    for (int j = 0; j < 2; j++) {
        // both sides of the flag
        qreal depth = (j==0)?0.01f:-0.01f;
        if (this->_type == 0) {
            this->color(0, 0, 1);
            glBegin(GL_TRIANGLES);
            // Vasen ala;

            this->vertex(0, 4.0f, depth);
            this->vertex(18.0f, 4.0f, depth);
            this->vertex(0, 7.0f, depth);
            this->vertex(0, 7.0f, depth);
            this->vertex(18.0f, 7.0f, depth);
            this->vertex(18.0f, 4.0f, depth);

            this->vertex(5.0f, 0, depth);
            this->vertex(8.0f, 0, depth);
            this->vertex(5.0f, 11.0f,depth);
            this->vertex(5.0f, 11.0f,depth);
            this->vertex(8.0f, 11.0f,depth);
            this->vertex(8.0f, 0, depth);


            glEnd();
        } else if (this->_type == 1) {

            this->color(1,0,0);

            glBegin(GL_POLYGON);

            int r = 3;

            for (int i=0; i < 360; i++) {
                float rad = i*M_PI/180;
                this->vertex(9.0f, 5.5f, depth);
                this->vertex(cos(rad)*r+9.0f, sin(rad)*r+5.5f, depth);
                rad = (i+1)*M_PI/180;
                this->vertex(cos(rad)*r+9.0f, sin(rad)*r+5.5f, depth);
            }
            glEnd();
        } else {
            this->color(1,0,0);

            glBegin(GL_LINES);
            this->vertex(0,    0.1f, depth);
            this->vertex(17.9f, 11.0f, depth);
            this->vertex(18.0f, 10.9f, depth);
            this->vertex(0.1f, 0,    depth);

            this->vertex(18.0f, 0.1f, depth);
            this->vertex(0.1f, 11.0f, depth);
            this->vertex(0,    10.9f, depth);
            this->vertex(17.9f, 0,    depth);
            glEnd();
        }
    }
}

void FlagObject::clicked()
{
    qreal type = this->_type;
    type++;
    if (this->_type == 2) {
        type = 0;
    }
    this->setType(type);

}
