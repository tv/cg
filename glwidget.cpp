#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent)
{
    setMouseTracking(true);
    this->selectedIndex = 0;

    this->shadowMapCoef = 0.5;
    this->blurCoef      = 0.25;

    this->initializedFBO = false;
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


    int mapWidth = this->_screenWidth * this->shadowMapCoef,
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
    glGenerateMipmapEXT(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,0);

    if ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        qDebug() << "initialize after color OpenGL Error: " << QString((char*)errString);
    }


    // Fbo for those
    glGenFramebuffers(1, &this->fboId);
    glBindFramebuffer(GL_FRAMEBUFFER_EXT, this->fboId);

    glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, this->depthTextureId, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,GL_TEXTURE_2D, this->colorTextureId, 0);

    FBOstatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if(FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT) {
        qDebug() << "shadow map ERRORS!!";
    } else {
        qDebug() << "shadow success!";
    }

    glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);

    if ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        qDebug() << "initialize after unbind OpenGL Error: " << QString((char*)errString);
    }

    this->initializedFBO = true;


    // blur fbo
    glGenFramebuffersEXT(1, &this->blurFboId);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->blurFboId);

    glGenTextures(1, &this->blurFboIdColorTextureId);
    glBindTexture(GL_TEXTURE_2D, this->blurFboIdColorTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F_ARB, mapWidth*this->blurCoef, mapHeight*this->blurCoef, 0, GL_RGB, GL_FLOAT, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, this->blurFboIdColorTextureId, 0);

    FBOstatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if(FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT) {
        qDebug() << "blur ERRORS!!";
    }

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

}

void GLWidget::blurShadowMap(void)
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->blurFboId);
    glViewport(0,0, this->_screenWidth  * this->shadowMapCoef * this->blurCoef,
                    this->_screenHeight * this->shadowMapCoef * this->blurCoef);

    this->_blurProgram->bind();

    this->_blurProgram->setUniformValue("scale_uniform", 1.0/(_screenWidth * shadowMapCoef * blurCoef), 0.0);
    this->_blurProgram->setUniformValue("texture_source", 0);

    GLfloat cube_vertices[] = {
        // front
        -this->_screenWidth/2, -this->_screenHeight/2, 0, 0.0, 0.0,
         this->_screenWidth/2, -this->_screenHeight/2, 0, 1.0, 0.0,
        -this->_screenWidth/2,  this->_screenHeight/2, 0, 1.0, 1.0,
         this->_screenWidth/2,  this->_screenHeight/2, 0, 0.0, 1.0
      };

    GLuint vbo_cube;

    glGenBuffers(1, &vbo_cube);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cube);

    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);


    this->_blurProgram->enableAttributeArray("coord3d");
    this->_blurProgram->enableAttributeArray("tex2d");
    this->_blurProgram->setAttributeBuffer(
        "coord3d",
        GL_FLOAT,
        0 * sizeof(GLfloat),
        2,
        5*sizeof(GLfloat)
    );

    this->_blurProgram->setAttributeBuffer(
        "tex2d",
        GL_FLOAT,
        3 * sizeof(GLfloat),
        2,
        5*sizeof(GLfloat)
    );

    glDrawArrays(GL_TRIANGLES, 0, 20);



    QMatrix4x4 proj = QMatrix4x4(),
               view = QMatrix4x4(),
              model = QMatrix4x4();
    proj.ortho(-this->_screenWidth/2, this->_screenWidth/2, -this->_screenHeight/2, this->_screenHeight/2, 1, 20);
    model.translate(0,0,-5);
    view.setToIdentity();

    this->_blurProgram->setUniformValue("m", model);
    this->_blurProgram->setUniformValue("v", view);
    this->_blurProgram->setUniformValue("p", proj);

    glDrawArrays(GL_QUADS, 0, 20);

    glBindFramebuffer(GL_FRAMEBUFFER, this->fboId);

    glViewport(0,0, this->_screenWidth * this->shadowMapCoef,
               this->_screenHeight * this->shadowMapCoef);
    this->_blurProgram->setUniformValue("scale_uniform", 1.0/(_screenHeight * shadowMapCoef), 0.0);
    glBindTexture(GL_TEXTURE_2D, this->blurFboIdColorTextureId);

    glDrawArrays(GL_TRIANGLES, 0, 20);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    this->_blurProgram->release();
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
    Material* material = new Material();

    FileObject* obj = new FileObject();
    obj->readFile("rock.obj");
    obj->setPosition(QVector3D(0, -2, -22));
    obj->setMaterial(material);
    this->_append(obj);
/*

    FileObject* obj2 = new FileObject();
    obj2->readFile("rock.obj");
    obj2->setPosition(QVector3D(-5, 0, -22));
    obj2->setMaterial(testMaterial);
    obj2->setRotateZ(15.0);
    this->_append(obj2);*/

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
}

void GLWidget::paintGL()
{
    GLenum errCode;
    const GLubyte *errString;




    if (!this->initializedFBO) {
        this->initializeFBO();
        qDebug() << "INITIALIZED!!";
    }
    // First pass for shadowmapping

    if ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        qDebug() << "First pass before fb bind OpenGL Error: " << QString((char*)errString) << this->fboId;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, this->fboId);

    if ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        qDebug() << "First pass after fb bind OpenGL Error: " << QString((char*)errString) << this->fboId;
    }

    glViewport(0,0, this->_screenWidth*shadowMapCoef, this->_screenHeight*shadowMapCoef);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



    if ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        qDebug() << "First pass before shader bind OpenGL Error: " << QString((char*)errString);
    }

    this->_shadowProgram->bind();

    QMatrix4x4 lightview = QMatrix4x4(),
               lightpersp = QMatrix4x4(),
               //bias = QMatrix4x4();
               bias = QMatrix4x4(0.5, 0.0, 0.0, 0.0,
                                 0.0, 0.5, 0.0, 0.0,
                                 0.0, 0.0, 0.5, 0.0,
                                 0.5, 0.5, 0.5, 1.0);

    //bias.setToIdentity();
    lightpersp.perspective(45.0f, //this->_lights.at(0)->spotCutOff,
                           this->_screenWidth/this->_screenHeight,
                           5.0f, 100.0f);



    lightview.lookAt(this->_lights.at(0)->position.toVector3D(),
                     this->_lights.at(0)->direction, QVector3D(0,1,0));

    this->_shadowProgram->setUniformValue("p", lightpersp);
    this->_shadowProgram->setUniformValue("v", lightview);

    if ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        qDebug() << "First pass before render OpenGL Error: " << QString((char*)errString);
    }
    glCullFace(GL_BACK);

    for (int i = 0; i < this->_objects.size(); i++) {
        this->_objects.at(i)->setShader(this->_shadowProgram);
        this->_objects.at(i)->draw();
    }

    if ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        qDebug() << "First pass before mipmap OpenGL Error: " << QString((char*)errString);
    }

    glGenerateMipmap(GL_TEXTURE_2D);






    // DEBUG only. this piece of code draw the depth buffer onscreen

     glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     this->_shadowProgram->release();
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
     glOrtho(-this->_screenWidth/2, this->_screenWidth/2, -this->_screenHeight/2, this->_screenHeight/2, 1, 20);
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

    //this->blurShadowMap();
    glActiveTexture(GL_TEXTURE7);



    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    if ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        qDebug() << "First pass end OpenGL Error: " << QString((char*)errString);
    }

    // Second pass

    glViewport(0,0, this->_screenWidth, this->_screenHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->_shadowProgram->release();
    this->_program->bind();
    // bind normal shaders

    GLfloat shMapStepX = 1.0/(this->_screenWidth * shadowMapCoef),
            shMapStepY = 1.0/(this->_screenHeight * shadowMapCoef);

    this->_program->setUniformValue("ShadowMap", 7);
    this->_program->setUniformValue("shadowMapStepX", shMapStepX);
    this->_program->setUniformValue("shadowMapStepY", shMapStepY);
    glActiveTextureARB(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, this->colorTextureId);


    glCullFace(GL_BACK);
    QMatrix4x4 realview = this->getView(),
               realpersp = this->getPerspective();



    this->_program->setUniformValue("p", realpersp);
    this->_program->setUniformValue("bias", bias * lightpersp * lightview);
    this->_program->setUniformValue("v", realview);
    this->_program->setUniformValue("v_inv", realview.inverted());


    for (int i = 0; i < this->_lights.size(); i++) {
        QString name = QString("in_light0");
        this->_lights.at(i)->outputFormat(this->_program, name);
    }

    for (int i = 0; i < this->_objects.size(); i++) {
        this->_objects.at(i)->setShader(this->_program);
        this->_objects.at(i)->draw();
    }

    this->_program->release();


    if ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        qDebug() << "second pass OpenGL Error: " << QString((char*)errString);
    }

    //QApplication::exit();
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

        qreal x = light->position.x() + dx,
              y = light->position.z() - dy;

        light->position.setX(x);
        light->position.setZ(y);
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


