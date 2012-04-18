#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent)
{
    setMouseTracking(true);
    this->selectedIndex = 1;
}

void GLWidget::initializeGL()
{
    const GLubyte* pGPU = glGetString(GL_RENDERER);
    const GLubyte* pVersion = glGetString(GL_VERSION);
    const GLubyte* pShaderVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    qDebug() << "GPU: " << QString((char*)pGPU).trimmed();
    qDebug() << "OpenGL: " << QString((char*)pVersion).trimmed();
    qDebug() << "GLSL: " << QString((char*)pShaderVersion);

//    glShadeModel(GL_SMOOTH);
    glClearColor (0.0, 0.0, 0.0, 0.0);
  //  glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    /*glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
*/
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
  //  glEnable(GL_NORMALIZE);
   //glEnable(GL_LIGHT0);
    //glEnable(GL_COLOR_MATERIAL);

    QGLShaderProgram* program = new QGLShaderProgram();
    this->_program = program;
    //this->_program->addShaderFromSourceFile(QGLShader::Geometry, "./shader.g.glsl");
    this->_program->addShaderFromSourceFile(QGLShader::Vertex, "./shader.v.glsl");
    this->_program->addShaderFromSourceFile(QGLShader::Fragment, "./shader.f.glsl");

    this->_program->link();
    this->_program->bind();

    qDebug() << this->_program->log();

    this->initializeObjects();

    this->_timer = new QTimer(this);
    connect(this->_timer, SIGNAL(timeout()), this, SLOT(animate()));
    this->_timer->start(10);

}

void GLWidget::_append(QLObject* obj)
{
    obj->setShader(this->_program);
    this->_objects.append(obj);
    QObject::connect(obj, SIGNAL(redraw()), this, SLOT(updateGL()));
    QObject::connect(this, SIGNAL(animateChild()), obj, SLOT(animate()));
}

void GLWidget::initializeObjects()
{
    MaterialTest* testMaterial = new MaterialTest();
    Material* material = new Material();

    FileObject* obj = new FileObject();
    obj->readFile("rock.obj");
    obj->setPosition(QVector3D(0, 1, -22));
    obj->setMaterial(material);
    this->_append(obj);


    FileObject* obj2 = new FileObject();
    obj2->readFile("rock.obj");
    obj2->setPosition(QVector3D(-5, 0, -22));
    obj2->setMaterial(testMaterial);
    obj2->setRotateZ(15.0);
    this->_append(obj2);

    FileObject* room = new FileObject();
    room->readFile("room.obj");
    room->setPosition(QVector3D(0, -5, -25));
    room->setScale(7.0f);
    room->setMaterial(material);
    this->_append(room);


    Light* light = new Light();
    this->_lights.append(light);

    QObject::connect(light, SIGNAL(redraw()), this, SLOT(updateGL()));
    QObject::connect(this, SIGNAL(animateChild()), light, SLOT(animate()));
}

QMatrix4x4 GLWidget::getPerspective()
{
    QMatrix4x4 persp = QMatrix4x4();
    persp.setToIdentity();
    persp.perspective(45.0f, this->_screenWidth/this->_screenHeight, 5.0f, 100.0f);

    return persp;
}

QMatrix4x4 GLWidget::getView()
{
    QMatrix4x4 view = QMatrix4x4();
    view.lookAt(
         QVector3D(0.0, 0.0, 0.0),
         QVector3D(0.0, 0.0, -100.0),
         QVector3D(0.0, 1.0, 0.0)
    );
    return view;
}

void GLWidget::resizeGL(int w, int h)
{
    this->_screenWidth = (double)w;
    this->_screenHeight = (double)h;

    QMatrix4x4 persp = this->getPerspective();
    this->_program->setUniformValue("p", persp);

    glViewport(0,0, w, h);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    QMatrix4x4 view = this->getView();
    this->_program->setUniformValue("v", view);

    this->_program->setUniformValue("v_inv", view.inverted());

    for (int i = 0; i < this->_lights.size(); i++) {
        QString name = QString("in_light0");
        this->_lights.at(i)->outputFormat(this->_program, name);
    }

    for (int i = 0; i < this->_objects.size(); i++) {
        this->_objects.at(i)->draw();
    }

    GLenum errCode;
    const GLubyte *errString;
    if ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        qDebug() << "OpenGL Error: " << QString((char*)errString);
    }

}

void GLWidget::animate()
{

    emit animateChild();
    this->updateGL();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton or event->button() == Qt::RightButton){
        this->checkClick(event);
    }
}

void GLWidget::checkClick(QMouseEvent *event)
{

    QMatrix4x4 view = this->getView();
    QMatrix4x4 perspective = this->getView();

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    GLfloat zNear = 0.0f,
            zFar  = 1.0f;

    qreal w = this->_screenWidth,
          h = this->_screenHeight,
          X = (event->x()-w/2)/(w/2),
          Y = -(event->y()-h/2)/(h/2);


    QVector3D rayVector;
    QVector4D nearPoint, farPoint;


    QMatrix4x4 invPV = (view*perspective).inverted();
    QVector4D  out = QVector4D(X, Y, zNear, 1.0);

    nearPoint = invPV.map(out);

    out = QVector4D(X, Y, zFar, 1.0);

    farPoint = invPV.map(out);

    rayVector = farPoint.toVector3D() - nearPoint.toVector3D();

    rayVector.normalize();

    qreal minZ = 100.0f, z;

    for (int i = 0, length = this->_objects.size(); i < length; i++) {
        QLObject* object = this->_objects.at(i);
        z = object->isHit(rayVector, minZ);
        if (z < minZ) {

            qDebug() << z << i;
            minZ = z;
            this->selectedIndex = i;

        }
    }

    if (minZ < 100.0f &&event->buttons() & Qt::LeftButton) {
        this->_objects.at(this->selectedIndex)->clicked();
    } else {
        qDebug() << "NO HIT :/!!" << nearPoint;
    }


    return;
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (this->selectedIndex == -1) return;

    if (event->buttons() & Qt::MidButton) {
        int dx = event->x() - lastPos.x();
        int dy = event->y() - lastPos.y();

        Light* light = this->_lights.at(0);

        qreal x = light->direction.x() + dx,
              y = light->direction.y() + dy;

        light->direction.setX(x);
        light->direction.setY(y);
        this->updateGL();
        //this->_objects.at(this->selectedIndex)->addRotate(dx, dy, 0);
    }

    if (event->buttons() & Qt::RightButton) {

        int dx = event->x() - lastPos.x();
        int dy = event->y() - lastPos.y();

        this->_objects.at(this->selectedIndex)->addRotate(dx, dy, 0);
        /*
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT,viewport);

        QVector3D cursor;

        qreal depth = (0-this->_objects.at(this->selectedIndex)->getDepth());

        qreal ydepth= depth*tanf(45.0f * M_PI / 360.0)*2;
        qreal xdepth = ydepth * ((double) viewport[2] / (double) viewport[3]);


        cursor.setX((double) event->x() / (double)viewport[2] * xdepth - xdepth/2);
        cursor.setY((double) (viewport[3] - event->y()) / (double)viewport[3] * ydepth -ydepth/2);
        cursor.setZ(depth);

        this->_objects.at(this->selectedIndex)->setPosition(cursor);*/
    }
    lastPos = event->pos();
}
/*
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
*/

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
            this->_lights.at(0)->spotCutOff++;
            break;
        case Qt::Key_Down:
            this->_lights.at(0)->spotCutOff--;
            break;
        case Qt::Key_Left:
            this->_objects.at(2)->addRotateX(15.0f);
            break;
        case Qt::Key_Right:
            this->_objects.at(2)->addRotateZ(15.0f);
            break;
        case Qt::Key_N:
            this->_objects.at(2)->addRotateY(45.0f);
            break;
        case Qt::Key_D:
            break;
        default:
            event->ignore();
            break;
    }
}


