#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent) {
    setMouseTracking(true);

    FlagObject* flag = new FlagObject();
    flag->setPosition(QVector3D(0, -2, -10));
    flag->setRotate(-22, 0, 0, 1);
    flag->setScale(.125);
    flag->setType(0);
    this->_objects.append(flag);

    FlagObject* flag2 = new FlagObject();
    flag2->setPosition(QVector3D(-3, 0, -10));
    flag2->setScale(0.25);
    flag2->setRotate(15.0, 0, 0, 1);
    flag2->setType(1);
    this->_objects.append(flag2);
}

void GLWidget::initializeGL()
{
    glShadeModel(GL_SMOOTH);
    glClearColor(0.1f,0.1f,0.1f,0.1f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

}

void GLWidget::resizeGL(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f,(double)w/(double)h,10.0f,100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


    //glLoadIdentity();
    for (int i = 0; i < this->_objects.size(); i++) {
        glLoadName(i+1);
        this->_objects.at(i)->draw();
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    std::cout << "ASD" << std::endl;
    if (event->button() == Qt::LeftButton){
        this->checkClick(event);
    }
}

#define SELECT_BUFSIZE 512

void GLWidget::checkClick(QMouseEvent *event)
{
    GLuint select_buf[SELECT_BUFSIZE];
    GLint viewport[4];
    GLuint hits;

    glSelectBuffer(SELECT_BUFSIZE, select_buf);
    glRenderMode(GL_SELECT);

    glInitNames();
    glPushName(0);

    glMatrixMode(GL_PROJECTION);

    glPushMatrix();
    glLoadIdentity();

    glGetIntegerv(GL_VIEWPORT,viewport);
    gluPickMatrix(
            event->x(),
            viewport[3] - event->y(),
            1.0f,
            1.0f,
            viewport
    );
    gluPerspective(
            45.0f,
            (viewport[2]-viewport[0]) / (viewport[3]-viewport[1]),
            0.1f,
            100.0f
    );

    std::cout << "x: " << viewport[0] << std::endl;
    std::cout << "y: " << viewport[1] << std::endl;
    std::cout << "x2: " << viewport[2] << std::endl;
    std::cout << "y2: " << viewport[3] << std::endl;

    glMatrixMode(GL_MODELVIEW);

    this->paintGL();

    glMatrixMode(GL_PROJECTION);                        // Select The Projection Matrix
    glPopMatrix();                              // Pop The Projection Matrix
    glMatrixMode(GL_MODELVIEW);
    glFlush();

    hits = glRenderMode(GL_RENDER);

    if (hits > 0) {
        int choose = select_buf[3];
        int depth = select_buf[1];

        // any other hits?
        for (int i = 1; i < hits; i++) {
            if (select_buf[i*4+1] < GLuint(depth)) {
                choose = select_buf[i*4+3];
                depth = select_buf[i*4+1];
            }
        }
        std::cout << choose << std::endl;
        this->_objects.at(choose-1)->setScale(0.75);
        this->update();
    }

    std::cout << hits << std::endl;
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    //printf("%d, %d\n", event->x(), event->y());
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
        case Qt::Key_Escape:
            close();
            break;
        default:
            event->ignore();
            break;
    }
}


