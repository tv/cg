#include "lightobject.h"

LightObject::LightObject(QObject *parent) :
    QLObject(parent)
{
    this->ignoreClickEvent = false;
}

void LightObject::draw() {

    glPushMatrix();

    this->move();
    this->rotate();
    GLfloat specular[] = {1.0f, 1.0f, 1.0f , 1.0f};

    glColor3f(1.0,1.0,1.0);
    
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    glBegin(GL_TRIANGLE_FAN);     	// draw triangle
    glVertex3f(  0.0f,  1.5f, 0.0f);
    glVertex3f(-2.5f, -2.5f, 2.5f);
    glVertex3f( 2.5f, -2.5f, 2.5f);
    glVertex3f( 2.5f, -2.5f, -2.5f);
    glVertex3f( -2.5f, -2.5f, -2.5f);
    glVertex3f(-2.5f, -2.5f, 2.5f);
    glEnd();
    glBegin(GL_QUADS);                           	// draw square
    glVertex3f(-2.5f, -2.5f, 2.5f);
    glVertex3f( -2.5f, -2.5f, -2.5f);
    glVertex3f( 2.5f, -2.5f, -2.5f);
    glVertex3f( 2.5f, -2.5f, 2.5f);
    glEnd();

    glPopMatrix();
}
