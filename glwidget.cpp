#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent) {
    setMouseTracking(true);
    this->selectedIndex = -1;

    TextObject* text = new TextObject();
    text->setParent(this);
    this->_objects.append(text);

    FlagObject* flag = new FlagObject();
    flag->setPosition(QVector3D(0, -2, -18));
    flag->setRotateZ(-10);
    flag->setScale(.25);
    flag->setType(0);
    this->_objects.append(flag);

    QObject::connect(flag, SIGNAL(redraw()), this, SLOT(updateGL()));

    FlagObject* flag2 = new FlagObject();
    flag2->setPosition(QVector3D(-3, 0, -22));
    flag2->setScale(0.25);
    flag2->setRotateZ(15.0);
    flag2->setType(1);
    this->_objects.append(flag2);
    QObject::connect(flag2, SIGNAL(redraw()), this, SLOT(updateGL()));

    this->nextDepth = -23;

    this->camera = QVector3D(0,0,0);
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

    glViewport(0,0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f,(double)w/(double)h,9.0f,100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < this->_objects.size(); i++) {
        glLoadName(i+1);
        this->_objects.at(i)->draw();
    }

    glRotatef(this->camera.x(), 0, 1, 0);
    glRotatef(this->camera.y(), 1, 0, 0);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton or event->button() == Qt::RightButton){
        this->checkClick(event);
    }
}

void GLWidget::checkClick(QMouseEvent *event)
{
    GLuint select_buf[512];
    GLint viewport[4];
    GLuint hits;

    glSelectBuffer(512, select_buf);
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
            (double)viewport[2] / (double)viewport[3],
            0.1f,
            100.0f
    );

    glMatrixMode(GL_MODELVIEW);

    this->paintGL();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glFlush();

    hits = glRenderMode(GL_RENDER);

    if (hits > 0) {
        int choose = select_buf[3];
        int depth = select_buf[1];

        if (this->_objects.at(choose-1)->ignoreClickEvent) {
            depth = -100;
        }
        // any other hits?
        for (int i = 1; i < hits; i++) {
            if (select_buf[i*4+1] < GLuint(depth)) {
                choose = select_buf[i*4+3];
                depth = select_buf[i*4+1];
            }
        }

        this->selectedIndex = choose-1;
        this->lastPos = event->pos();

        if (event->buttons() & Qt::LeftButton) {            
            this->_objects.at(this->selectedIndex)->clicked();
        }
    }

    std::cout << hits << std::endl;
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (this->selectedIndex == -1) return;

    if (event->buttons() & Qt::MidButton) {
        int dx = event->x() - lastPos.x();
        int dy = event->y() - lastPos.y();

        this->_objects.at(this->selectedIndex)->addRotate(dx, dy, 0);
    }

    if (event->buttons() & Qt::RightButton) {
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT,viewport);

        int dx = event->x() - lastPos.x();
        int dy = event->y() - lastPos.y();

        qreal depth = (0-this->_objects.at(this->selectedIndex)->getDepth());

        qreal ydepth= depth*tanf(45.0f * M_PI / 360.0)*2;
        qreal xdepth = ydepth * ((double) viewport[2] / (double) viewport[3]);


        this->_objects.at(this->selectedIndex)->setPosition(
            (double) event->x() / (double)viewport[2] * xdepth - xdepth/2,
            (double) (viewport[3] - event->y()) / (double)viewport[3] * ydepth -ydepth/2
        );
    }
    lastPos = event->pos();
}

void GLWidget::rotateX(int rotate)
{
    if (this->selectedIndex == -1) return;

    this->_objects.at(this->selectedIndex)->addRotateX(rotate);
}

void GLWidget::rotateZ(int rotate)
{
    if (this->selectedIndex == -1) return;

    this->_objects.at(this->selectedIndex)->addRotateZ(rotate);
}

void GLWidget::addNewFlag()
{
    srand ( time(NULL) );
    FlagObject* flag = new FlagObject();
    flag->setPosition(QVector3D(rand() % 10 - 4, rand() % 10 - 4, this->nextDepth));
    flag->setRotateZ(-10);
    flag->setScale(.25);
    flag->setType(0);
    this->_objects.append(flag);

    QObject::connect(flag, SIGNAL(redraw()), this, SLOT(updateGL()));

    this->nextDepth--;
    this->updateGL();
}

void GLWidget::deleteSelectedFlag()
{
    if (this->selectedIndex == -1) return;

    this->_objects.removeAt(this->selectedIndex);

    this->selectedIndex = -1;

    this->updateGL();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    if (this->selectedIndex == -1) return;

    //if (event->orientation() == Qt::Horizontal) {
        int numDegrees = event->delta() / 8;
        int numSteps = numDegrees / 15;
        this->_objects.at(this->selectedIndex)->addDepth(numSteps);
    //}
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
        case Qt::Key_Escape:
            QApplication::exit();
            QApplication::quit();
            break;
        case Qt::Key_Up:
            this->rotateX(5);
            break;
        case Qt::Key_Down:
            this->rotateX(-5);
            break;
        case Qt::Key_Left:
            this->rotateZ(-5);
            break;
        case Qt::Key_Right:
            this->rotateZ(5);
            break;
        case Qt::Key_N:
            this->addNewFlag();
            break;
        case Qt::Key_D:
            this->deleteSelectedFlag();
            break;
        default:
            event->ignore();
            break;
    }
}


