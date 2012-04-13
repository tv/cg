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

    glMaterialf(GL_FRONT, GL_SHININESS, 25.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

    glEnable(GL_LIGHT0);

    glBegin(GL_TRIANGLE_FAN);     	// draw triangle
    this->vertex(0.0f,  1.5f, 0.0f);
    this->vertex(-2.5f, -2.5f, 2.5f);
    this->vertex( 2.5f, -2.5f, 2.5f);
    this->vertex( 2.5f, -2.5f, -2.5f);
    this->vertex( -2.5f, -2.5f, -2.5f);
    this->vertex(-2.5f, -2.5f, 2.5f);
    glEnd();
    glBegin(GL_QUADS);                           	// draw square
    this->vertex(-2.5f, -2.5f, 2.5f);
    this->vertex( -2.5f, -2.5f, -2.5f);
    this->vertex( 2.5f, -2.5f, -2.5f);
    this->vertex( 2.5f, -2.5f, 2.5f);
    glEnd();

    this->drawBoundingBox();

    glPopMatrix();
}
