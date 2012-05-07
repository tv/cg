#include "glwidget.h"

GLWidget::GLWidget(QGLFormat format, QWidget *parent)
    : QGLWidget(format, parent)
{

    setMouseTracking(true);
    this->selectedIndex = 0;

    this->showDebug = false;

    this->debugMode = 0;
    this->shadowEnabled = true;
}

void GLWidget::initializeGL()
{
    GLenum errCode;

    if ((errCode = glGetError()) != GL_NO_ERROR) {
        qDebug() << "initializeGL start OpenGL Error: " << errCode;
    }


    const GLubyte* pGPU = glGetString(GL_RENDERER);
    const GLubyte* pVersion = glGetString(GL_VERSION);
    const GLubyte* pShaderVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    qDebug() << "GPU: " << QString((char*)pGPU).trimmed();
    qDebug() << "OpenGL: " << QString((char*)pVersion).trimmed();
    qDebug() << "GLSL: " << QString((char*)pShaderVersion);

    glClearColor (0.0, 0.0, 0.0, 0.0);

    glEnable(GL_DEPTH_TEST);

    if ((errCode = glGetError()) != GL_NO_ERROR) {
        qDebug() << "initializeGL after enable OpenGL Error: " << errCode;
    }


    this->_program = new QGLShaderProgram();
    this->_program->addShaderFromSourceFile(QGLShader::Vertex, "./shaders/shader.v.glsl");
    this->_program->addShaderFromSourceFile(QGLShader::Fragment, "./shaders/shader.f.glsl");

    this->_shadowProgram = new QGLShaderProgram();
    this->_shadowProgram->addShaderFromSourceFile(QGLShader::Vertex, "./shaders/shadow.v.glsl");
    this->_shadowProgram->addShaderFromSourceFile(QGLShader::Fragment, "./shaders/shadow.f.glsl");


    if ((errCode = glGetError()) != GL_NO_ERROR) {
        qDebug() << "initializeGL after shaders OpenGL Error: " << errCode;
    }


    this->_program->link();
    this->_shadowProgram->link();

    qDebug() << this->_program->log();
    qDebug() << this->_shadowProgram->log();

    this->camera = new Camera();

    if ((errCode = glGetError()) != GL_NO_ERROR) {
        qDebug() << "initializeGL before objects OpenGL Error: " << errCode;
    }

    this->initializeObjects();

    if ((errCode = glGetError()) != GL_NO_ERROR) {
        qDebug() << "initializeGL END OpenGL Error: " << errCode;
    }


    this->_timer = new QTimer(this);
    connect(this->_timer, SIGNAL(timeout()), this, SLOT(animate()));
    this->_timer->start(100);

}

void GLWidget::paintGL()
{
    GLenum errCode;

    // First pass for shadowmapping

    QGLShaderProgram * p;
    Camera * cam;
    Light * light;


    if (this->debugMode != 3) {



    for (int i = 0; i < this->_lights.size(); i++) {
        light = this->_lights.at(i);
        if (!light->initializedFBO) {
            light->initializeShadowFBO(QSize(this->_screenWidth, this->_screenHeight));
            return;
        }

        if (this->debugMode != 4) {
            light->bindFBO();
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glCullFace(GL_FRONT);

        if (this->debugMode == 4) {

            p = this->_program;
            p = this->_shadowProgram;


        } else {

            glViewport(0,0, this->_screenWidth/2, this->_screenHeight/2);
            p = this->_shadowProgram;

        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cam = this->_lights.at(i)->camera;

        p->bind();

        // wont be used in shadow program
        p->setUniformValue("debug_mode", 4);

        cam->injectToShader(p);

        for (int i = 0; i < this->_objects.size(); i++) {
            this->_objects.at(i)->injectToShader(p);
        }

        if (this->debugMode == 4) {
            p->release();
            return;
        }

        light->releaseFBO();

        p->release();
    }
    }

    if (this->showDebug) {

        // DEBUG only. this piece of code draw the depth buffer onscreen

        glBindFramebuffer(GL_FRAMEBUFFER,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        this->_shadowProgram->release();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-this->_screenWidth/2, this->_screenWidth/2, -this->_screenHeight/2, this->_screenHeight/2, 1, 60);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glColor4f(1,1,1,1);

        this->_lights.at(0)->bindDebugShadowMap();

        glEnable(GL_TEXTURE_2D);
        glTranslated(0,0,-1);
        glBegin(GL_QUADS);
        glTexCoord2d(0,0);glVertex3f(0,0,0);
        glTexCoord2d(1,0);glVertex3f(_screenWidth/2,0,0);
        glTexCoord2d(1,1);glVertex3f(_screenWidth/2,_screenHeight/2,0);
        glTexCoord2d(0,1);glVertex3f(0,_screenHeight/2,0);
        glEnd();
        glDisable(GL_TEXTURE_2D);

        //glutSwapBuffers();
        return;
    }



    // Second pass
    p = this->_program;
    cam = this->camera;


    glViewport(0,0, this->_screenWidth, this->_screenHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if ((errCode = glGetError()) != GL_NO_ERROR) {
        qDebug() << "GLWidget::draw after clear: " << errCode;
    }

    p->bind();

    cam->injectToShader(p);
    if ((errCode = glGetError()) != GL_NO_ERROR) {

        qDebug() << "GLWidget::draw  after cam: " << errCode;
    }

    p->setUniformValue("debug_mode", this->debugMode);

    for (int i = 0; i < this->_lights.size(); i++) {
        this->_lights.at(i)->injectToShader(p, i);
    }

    for (int i = 0; i < this->_objects.size(); i++) {
        this->_objects.at(i)->injectToShader(p);
    }

    this->_program->release();

    if ((errCode = glGetError()) != GL_NO_ERROR) {

        qDebug() << "GLWidget  after  2nd pas: " << errCode;
    }

}

void GLWidget::_append(QLObject* obj)
{
    this->_objects.append(obj);
    QObject::connect(obj, SIGNAL(redraw()), this, SLOT(updateGL()));
    QObject::connect(this, SIGNAL(animateChild()), obj, SLOT(animate()));
}

void GLWidget::initializeObjects()
{

    Material* material = new Material();
    material->setSpecular(QVector4D(1.0, 1.0, 1.0, 1.0));
    material->setShihiness(70.0f);
    Material* material2 = new Material();
    material2->setDiffuse(QVector4D(0.3, 0.3, 1.0, 1.0));
    Material* material3 = new Material();
    material3->setAmbient(QVector4D(0.4, 1.0, 0.3, 1.0));

    GLenum errCode;
    if ((errCode = glGetError()) != GL_NO_ERROR) {
        qDebug() << "initObjects after mat OpenGL Error: " << errCode;
    }

    GridNStuff* g = new GridNStuff();
    g->lineAA(QVector3D(75, 50, -25), QVector3D(-75, -50, 25));
    g->lineAA(QVector3D(50, 50, 50), QVector3D(-50, -50, -50));
    g->circleMid(QVector2D(0,0), 50);

    g->setPosition(QVector3D(3, 0, -15));
    g->setScale(3.0f);
    g->setMaterial(material2);

    g->gridPlane();
    this->_append(g);



    FileObject* monkey = new FileObject();
    monkey->readFile("textured.obj");
    monkey->setPosition(QVector3D(-3, 0, -15));
    this->_append(monkey);

    this->_append(monkey);


    FileObject* room = new FileObject();
    room->readFile("floor.obj");
    room->setPosition(QVector3D(0, -4, -15));
    this->_append(room);



    if ((errCode = glGetError()) != GL_NO_ERROR) {
        qDebug() << "initObjects after append OpenGL Error: " << errCode;
    }

    Light* light = new Light();
    Camera* cam = new Camera();
    cam->position = QVector4D(0.0f, 15.0f, -10.0f, 1.0f);
    cam->theta = -45.0f;
    cam->beta = 0.0f;
    cam->calculateDirection();
    light->setCamera(cam);
    this->_lights.append(light);


    QObject::connect(light, SIGNAL(redraw()), this, SLOT(updateGL()));
    QObject::connect(this, SIGNAL(animateChild()), light, SLOT(animate()));

    if ((errCode = glGetError()) != GL_NO_ERROR) {
        qDebug() << "initObjects END OpenGL Error: " << errCode;
    }
}

void GLWidget::resizeGL(int w, int h)
{

    this->_screenWidth = (double)w;
    this->_screenHeight = (double)h;

    qreal aspect = this->_screenWidth/this->_screenHeight;

    this->camera->setPerspective(45.0f, aspect, 1.0f, 100.0f);

    for (int i = 0; i < this->_lights.size(); i++) {
        this->_lights.at(i)->updateProjection(aspect);
        this->_lights.at(i)->initializedFBO = false;
    }
}

void GLWidget::animate()
{
    emit animateChild();
    this->updateGL();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    //if (event->button() == Qt::LeftButton or event->button() == Qt::RightButton){
    //    this->checkClick(event);
    //}
}

void GLWidget::checkClick(QMouseEvent *event)
{

    QMatrix4x4 view = this->camera->getMatrix();
    QMatrix4x4 perspective = this->camera->projection;

    GLfloat zNear = 0.0f,
            zFar  = 50.0f;

    qreal w = this->_screenWidth,
          h = this->_screenHeight,
          X = (event->x()-w/2)/(w/2),
          Y = -(event->y()-h/2)/(h/2);


    QVector3D rayVector;
    QVector4D nearPoint, farPoint;


    QMatrix4x4 invPV = (view*perspective).inverted();
    QVector4D  out = QVector4D(X, Y, zNear, 1.0);

    nearPoint = invPV * out;

    out = QVector4D(X, Y, zFar, 1.0);

    farPoint = invPV * out;

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

    if (event->buttons() & Qt::LeftButton) {

        int dx = event->x() - lastPos.x();
        int dy = event->y() - lastPos.y();

        this->_objects.at(1)->addRotate(dx, dy, 0);

        return;
/*
        int dx = event->x() - lastPos.x();
        int dy = event->y() - lastPos.y();



        if (dx > 0) {
            this->_lights.at(0)->camera->rotateRight();
        } else if (dx < 0) {
            this->_lights.at(0)->camera->rotateLeft();
        }

        if (dy > 0) {
            this->_lights.at(0)->camera->rotateUp();
        } else  if (dy < 0) {
            this->_lights.at(0)->camera->rotateDown();
        }*/
    }

    if (event->buttons() & Qt::MidButton) {

        int dx = event->x() - lastPos.x();
        int dy = event->y() - lastPos.y();


        if (dx > 0) {
            this->camera->rotateRight();
        } else if (dx < 0) {
            this->camera->rotateLeft();
        }

        if (dy > 0) {
            this->camera->rotateUp();
        } else  if (dy < 0) {
            this->camera->rotateDown();
        }
        //this->_objects.at(this->selectedIndex)->addRotate(dx, dy, 0);*/
    }

    if (event->buttons() & Qt::RightButton) {

        int dx = event->x() - lastPos.x();
        int dy = event->y() - lastPos.y();

        this->_objects.at(this->selectedIndex)->addRotate(dx, dy, 0);
    }
    lastPos = event->pos();
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
    Camera* cam = this->camera;
    if (this->debugMode == 4) {
        cam = this->_lights.at(0)->camera;
    }
    switch(event->key()) {
        case Qt::Key_Escape:
            QApplication::exit();
            QApplication::quit();
            break;
        case Qt::Key_Up:
            cam->moveForward();
            break;
        case Qt::Key_Down:
            cam->moveBack();
            break;
        case Qt::Key_Left:
            cam->strafeLeft();
            break;
        case Qt::Key_Right:
            cam->strafeRight();
            break;
        case Qt::Key_Z:
            cam->moveDown();
            break;
        case Qt::Key_A:
            cam->moveUp();
            break;
        case Qt::Key_Q:
            if (this->shadowEnabled) {
                this->shadowEnabled = false;
            } else {
                this->shadowEnabled = true;
            }
            break;
        case Qt::Key_W:
            if (this->debugMode <= 3) {
                this->debugMode++;
            } else {
                this->debugMode = 0;
            }
            if (this->debugMode == 0) {
                qDebug() << this->debugMode << "Debug: Disabled";
            } else if ( this->debugMode == 1) {
                qDebug() << this->debugMode << "Debug: Show light cobinations, red = diff+spec, green = attenuation, blue = shadow";
            } else if ( this->debugMode == 2) {
                qDebug() << this->debugMode << "Debug: force color to white";
            } else if ( this->debugMode == 3) {
                qDebug() << this->debugMode << "Debug: disable shadow";
            } else if ( this->debugMode == 4) {
                qDebug() << this->debugMode << "Debug: Lights pow";
            }

        case Qt::Key_N:

            break;
        case Qt::Key_D:
            if (this->showDebug) {
                this->showDebug = false;
            } else {
                this->showDebug = true;
            }
            break;

        case Qt::Key_8:
            this->_lights.at(0)->camera->moveUp();
            break;
        case Qt::Key_2:
            this->_lights.at(0)->camera->moveDown();
            break;
        default:
            event->ignore();
            break;
    }
}


