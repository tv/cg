#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent)
{
    setMouseTracking(true);
    this->selectedIndex = 0;

    this->showDebug = false;

    this->debugMode = 0;
    this->shadowEnabled = true;
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


    this->_program = new QGLShaderProgram();
    //this->_program->addShaderFromSourceFile(QGLShader::Geometry, "./shader.g.glsl");
    this->_program->addShaderFromSourceFile(QGLShader::Vertex, "./shader.v.glsl");
    this->_program->addShaderFromSourceFile(QGLShader::Fragment, "./shader.f.glsl");

    this->_shadowProgram = new QGLShaderProgram();
    this->_shadowProgram->addShaderFromSourceFile(QGLShader::Vertex, "./shadow.v.glsl");
    this->_shadowProgram->addShaderFromSourceFile(QGLShader::Fragment, "./shadow.f.glsl");

    this->_program->link();
    this->_shadowProgram->link();

    qDebug() << this->_program->log();
    qDebug() << this->_shadowProgram->log();

    this->camera = new Camera();
    this->initializeObjects();

    GLenum errCode;
    const GLubyte *errString;
    if ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        qDebug() << "initializeGL END OpenGL Error: " << QString((char*)errString);
    }




    this->_timer = new QTimer(this);
    connect(this->_timer, SIGNAL(timeout()), this, SLOT(animate()));
    this->_timer->start(10);

}
/*
void GLWidget::initializeFBO()
{


    int mapWidth = this->_screenWidth  * this->shadowMapCoef,
        mapHeight = this->_screenHeight * this->shadowMapCoef;


    GLenum errCode;
    const GLubyte *errString;
    GLenum FBOstatus;

    glGenFramebuffers(1, &this->fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, this->fboId);

    // Depth texture
    glGenTextures(1, &this->depthTextureId);
    glBindTexture(GL_TEXTURE_2D, this->depthTextureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        mapWidth, mapHeight, 0,
        GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);



    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthTextureId, 0);

    // Fbo for those
    FBOstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT) {
        qDebug() << "shadow map ERRORS!!";
    } else {
        qDebug() << "shadow success!";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        qDebug() << "initialize after unbind OpenGL Error: " << QString((char*)errString);
    }

    this->initializedFBO = true;
}
*/


void GLWidget::paintGL()
{
    GLenum errCode;
    const GLubyte *errString;

    // First pass for shadowmapping

    QGLShaderProgram * p;
    Camera * cam;
    Light * light;

    // only one light atm
    for (int i = 0; i < this->_lights.size(); i++) {
        light = this->_lights.at(i);
        if (!light->initializedFBO) {
            light->initializeShadowFBO(QSize(this->_screenWidth, this->_screenHeight));
            return;
        }

        QApplication::exit();
        QApplication::quit();

        if (this->debugMode != 4) {
            light->bindFBO();
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glCullFace(GL_FRONT);

        if (this->debugMode == 4) {

            p = this->_program;
            p = this->_shadowProgram;


        } else {

            glViewport(0,0, this->_screenWidth*0.5, this->_screenHeight*0.5);
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
    p->bind();

    cam->injectToShader(p);

    p->setUniformValue("debug_mode", this->debugMode);

    for (int i = 0; i < this->_lights.size(); i++) {
        this->_lights.at(i)->injectToShader(p, i);
    }

    for (int i = 0; i < this->_objects.size(); i++) {
        this->_objects.at(i)->injectToShader(p);
    }

    this->_program->release();

    if ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        qDebug() << "GLWidget  after  2nd pas: " << QString((char*)errString);
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


    FileObject* cube = new FileObject();
    cube->readFile("rock.obj");
    cube->setPosition(QVector3D(0, 3, -22));
    cube->setMaterial(material);
    this->_append(cube);


    FileObject* room = new FileObject();
    room->setMaterial(material);
    room->readFile("floor.obj");
    room->setPosition(QVector3D(0, -5, -22));
    this->_append(room);

    FileObject* floor = new FileObject();
    floor->setMaterial(material);
    floor->readFile("floor.obj");
    floor->setPosition(QVector3D(0, 0, -27));
    floor->setRotateX(90.0f);
    this->_append(floor);


    Light* light = new Light();

    Camera* cam = new Camera();

    cam->position = QVector4D(0.0f, 15.0f, -10.0f, 1.0f);

    cam->theta = -30.0f;
    cam->beta = 0.0f;

    cam->calculateDirection();

    light->setCamera(cam);
    this->_lights.append(light);


    QObject::connect(light, SIGNAL(redraw()), this, SLOT(updateGL()));
    QObject::connect(this, SIGNAL(animateChild()), light, SLOT(animate()));
}

void GLWidget::resizeGL(int w, int h)
{

    this->_screenWidth = (double)w;
    this->_screenHeight = (double)h;

    qreal aspect = this->_screenWidth/this->_screenHeight;

    this->camera->setPerspective(45.0f, aspect, 1.0f, 100.0f);

    for (int i = 0; i < this->_lights.size(); i++) {
        this->_lights.at(i)->updateProjection(aspect);
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

    if (event->buttons() & Qt::LeftButton) {
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
        }
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


