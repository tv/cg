#include "lightobject.h"

LightObject::LightObject(QObject *parent) :
    QLObject(parent)
{
    this->ignoreClickEvent = false;
}

void LightObject::draw() {

    glPushMatrix();

    GLfloat diffuse[]={0.8, 0.8, 0.8, 1.0};
    GLfloat pos[]={0.5, 0.0, 0.8, 0.0};
    GLfloat dir[]={0.0, 0.0, 1.0};
    GLfloat cutOff = 75.0f;


    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.5, 0.0, 0.0);
    glVertex3f(0.5, 0.0, 20.0);
    glEnd();


    
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);
    glLightfv(GL_LIGHT0, GL_SPOT_CUTOFF, &cutOff),

    glEnable(GL_LIGHT0);

    this->drawBoundingBox();

    glPopMatrix();
}
