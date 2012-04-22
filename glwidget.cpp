#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent)
{
    setMouseTracking(true);
    this->selectedIndex = 0;

    this->shadowMapCoef = 0.5;
    this->blurCoef      = 0.25;

    this->initializedFBO = false;

    this->showDebug = false;

    this->debugMode = 0;
    this->blurEnabled = true;
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

    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    this->_program = new QGLShaderProgram();
    //this->_program->addShaderFromSourceFile(QGLShader::Geometry, "./shader.g.glsl");
    this->_program->addShaderFromSourceFile(QGLShader::Vertex, "./shader.v.glsl");
    this->_program->addShaderFromSourceFile(QGLShader::Fragment, "./shader.f.glsl");

    this->_shadowProgram = new QGLShaderProgram();
    this->_shadowProgram->addShaderFromSourceFile(QGLShader::Vertex, "./shadow.v.glsl");
    this->_shadowProgram->addShaderFromSourceFile(QGLShader::Fragment, "./shadow.f.glsl");

    this->_blurProgram = new QGLShaderProgram();
    this->_blurProgram->addShaderFromSourceFile(QGLShader::Vertex, "./blur.v.glsl");
    this->_blurProgram->addShaderFromSourceFile(QGLShader::Fragment, "./blur.f.glsl");

    this->_program->link();
    this->_shadowProgram->link();
    this->_blurProgram->link();

    qDebug() << this->_program->log();
    qDebug() << this->_shadowProgram->log();
    qDebug() << this->_blurProgram->log();

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

void GLWidget::initializeFBO()
{


    int mapWidth = this->_screenWidth  * this->shadowMapCoef,
        mapHeight = this->_screenHeight * this->shadowMapCoef;


    GLenum errCode;
    const GLubyte *errString;
    GLenum FBOstatus;

    // Depth texture
    glGenTextures(1, &this->depthTextureId);
    glBindTexture(GL_TEXTURE_2D, this->depthTextureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        mapWidth, mapHeight, 0,
        GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    // Color texture
    glGenTextures(1, &this->colorTextureId);
    glBindTexture(GL_TEXTURE_2D, this->colorTextureId);

    if ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        qDebug() << "initialize before parametri color OpenGL Error: " << QString((char*)errString);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    if ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        qDebug() << "initialize after color parametri OpenGL Error: " << QString((char*)errString);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F_ARB, mapWidth, mapHeight, 0, GL_RGB, GL_FLOAT, 0);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,0);

    if ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        qDebug() << "initialize after color OpenGL Error: " << QString((char*)errString);
    }


    // Fbo for those
    glGenFramebuffers(1, &this->fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, this->fboId);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthTextureId, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, this->colorTextureId, 0);

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


    // blur fbo
    glGenFramebuffers(1, &this->blurFboId);
    glBindFramebuffer(GL_FRAMEBUFFER, this->blurFboId);

    glGenTextures(1, &this->blurFboIdColorTextureId);
    glBindTexture(GL_TEXTURE_2D, this->blurFboIdColorTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F_ARB, mapWidth*this->blurCoef, mapHeight*this->blurCoef, 0, GL_RGB, GL_FLOAT, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->blurFboIdColorTextureId, 0);

    FBOstatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER);
    if(FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT) {
        qDebug() << "blur ERRORS!!";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLfloat cube_vertices[] = {
        // front
        -this->_screenWidth/2, -this->_screenHeight/2, 0, 0.0, 0.0,
         this->_screenWidth/2, -this->_screenHeight/2, 0, 1.0, 0.0,
        -this->_screenWidth/2,  this->_screenHeight/2, 0, 1.0, 1.0,
         this->_screenWidth/2,  this->_screenHeight/2, 0, 0.0, 1.0
      };

    this->blurVBO = new QGLBuffer(QGLBuffer::VertexBuffer);
    this->blurVBO->create();
    this->blurVBO->bind();
    this->blurVBO->allocate(cube_vertices, 4 * 5 * sizeof(GLfloat));
    this->blurVBO->release();

}

void GLWidget::blurShadowMap(void)
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->blurFboId);
    glViewport(0,0, this->_screenWidth  * this->shadowMapCoef * this->blurCoef,
                    this->_screenHeight * this->shadowMapCoef * this->blurCoef);

    this->_blurProgram->bind();

    this->_blurProgram->setUniformValue("scale_uniform", 1.0/(_screenWidth * shadowMapCoef * blurCoef), 0.0);
    this->_blurProgram->setUniformValue("texture_source", 0);




    this->blurVBO->bind();

    this->_blurProgram->enableAttributeArray("v_coord");
    this->_blurProgram->enableAttributeArray("v_tex");
    this->_blurProgram->setAttributeBuffer(
        "v_coord",
        GL_FLOAT,
        0 * sizeof(GLfloat),
        2,
        5*sizeof(GLfloat)
    );

    this->_blurProgram->setAttributeBuffer(
        "v_tex",
        GL_FLOAT,
        3 * sizeof(GLfloat),
        2,
        5*sizeof(GLfloat)
    );

    glDrawArrays(GL_QUADS, 0, 20);



    QMatrix4x4 proj = QMatrix4x4(),
               view = QMatrix4x4(),
              model = QMatrix4x4();
    proj.ortho(-this->_screenWidth/2, this->_screenWidth/2, -this->_screenHeight/2, this->_screenHeight/2, 1, 20);
    model.translate(0,0,-5);
    view.setToIdentity();

    this->_blurProgram->setUniformValue("m", model);
    this->_blurProgram->setUniformValue("v", view);
    this->_blurProgram->setUniformValue("p", proj);

    glDrawArrays(GL_QUADS, 0, this->blurVBO->size() / sizeof(GLfloat));

    glBindFramebuffer(GL_FRAMEBUFFER, this->fboId);

    glViewport(0,0, this->_screenWidth * this->shadowMapCoef,
               this->_screenHeight * this->shadowMapCoef);
    this->_blurProgram->setUniformValue("scale_uniform", 1.0/(_screenHeight * shadowMapCoef), 0.0);
    glBindTexture(GL_TEXTURE_2D, this->blurFboIdColorTextureId);

    glDrawArrays(GL_QUADS, 0, this->blurVBO->size() / sizeof(GLfloat));

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    this->_blurProgram->disableAttributeArray("v_coord");
    this->_blurProgram->disableAttributeArray("v_tex");
    this->_blurProgram->release();
    this->blurVBO->release();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void GLWidget::generateShadowMap()
{

}

void GLWidget::drawObjects(QGLShaderProgram * p, Camera *cam)
{
    cam->injectToShader(p);

    p->setUniformValue("debug_mode", this->debugMode);

    for (int i = 0; i < this->_lights.size(); i++) {
        this->_lights.at(i)->injectToShader(p, i);
    }

    for (int i = 0; i < this->_objects.size(); i++) {
        this->_objects.at(i)->injectToShader(p);
    }

}



void GLWidget::paintGL()
{
    GLenum errCode;
    const GLubyte *errString;

    if (!this->initializedFBO) {
        this->initializeFBO();
    }
    // First pass for shadowmapping

    QGLShaderProgram * p;
    Camera * cam;

    glBindFramebuffer(GL_FRAMEBUFFER, this->fboId);

    glViewport(0,0, this->_screenWidth*shadowMapCoef, this->_screenHeight*shadowMapCoef);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    p = this->_shadowProgram;
    cam = this->_lights.at(0)->camera;

    p->bind();

    cam->injectToShader(p);

    p->setUniformValue("debug_mode", this->debugMode);
/*
    for (int i = 0; i < this->_lights.size(); i++) {
        this->_lights.at(i)->injectToShader(p, i);
    }*/

    for (int i = 0; i < this->_objects.size(); i++) {
        this->_objects.at(i)->injectToShader(p);
    }


    glGenerateMipmap(GL_TEXTURE_2D);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    p->release();

    if (this->blurEnabled) {
        this->blurShadowMap();
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
        glActiveTextureARB(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,this->depthTextureId);
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

    if (this->shadowEnabled) {
        p->setUniformValue("ShadowMap", 7);
        glActiveTextureARB(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, this->colorTextureId);
    }

    glCullFace(GL_BACK);


    cam->injectToShader(p);

    p->setUniformValue("debug_mode", this->debugMode);

    for (int i = 0; i < this->_lights.size(); i++) {
        this->_lights.at(i)->injectToShader(p, i);
    }

    for (int i = 0; i < this->_objects.size(); i++) {
        this->_objects.at(i)->injectToShader(p);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
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

    FileObject* obj = new FileObject();
    obj->readFile("rock.obj");
    obj->setPosition(QVector3D(0, -2, -22));
    obj->setMaterial(material);
    this->_append(obj);

    FileObject* room = new FileObject();
    room->readFile("floor.obj");
    room->setMaterial(material);
    room->setPosition(QVector3D(0, -5, -25));
    room->setScale(10.0f);
    room->setMaterial(material);
    this->_append(room);


    Light* light = new Light();

    Camera * cam = new Camera();
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
        this->_lights.at(i)->camera->setPerspective(45.0f, aspect, 1.0f, 100.0f);
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
    switch(event->key()) {
        case Qt::Key_Escape:
            QApplication::exit();
            QApplication::quit();
            break;
        case Qt::Key_Up:
            this->camera->moveForward();
            break;
        case Qt::Key_Down:
            this->camera->moveBack();
            break;
        case Qt::Key_Left:
            this->camera->strafeLeft();
            break;
        case Qt::Key_Right:
            this->camera->strafeRight();
            break;
        case Qt::Key_Z:
            this->camera->moveDown();
            break;
        case Qt::Key_A:
            this->camera->moveUp();
            break;
        case Qt::Key_Q:
            if (this->shadowEnabled) {
                this->shadowEnabled = false;
            } else {
                this->shadowEnabled = true;
            }
            break;
        case Qt::Key_W:
            if (this->debugMode <= 2) {
                this->debugMode++;
            } else {
                this->debugMode = 0;
            }
            qDebug() << this->debugMode;
        case Qt::Key_N:
            if (this->blurEnabled) {
                this->blurEnabled = false;
            } else {
                this->blurEnabled = true;
            }
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


