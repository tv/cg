#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent) {
    setMouseTracking(true);
    this->selectedIndex = -1;

    this->nearPoint = QVector3D(0,0,0);
    this->farPoint = QVector3D(0,0,0);
/*
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
*/

    MaterialTest testMaterial;
    Material material;

    FileObject* file1 = new FileObject();
    file1->readFile("rock.obj");
    file1->setPosition(QVector3D(0, 5, -22));
    file1->setScale(1.5);
    file1->setMode(1);
    file1->setMaterial(material);
    this->_objects.append(file1);
    QObject::connect(file1, SIGNAL(redraw()), this, SLOT(updateGL()));

    FileObject* file2 = new FileObject();
    file2->readFile("rock.obj");
    file2->setPosition(QVector3D(-5, 0, -22));
    file2->setScale(1.5);
    file2->setMode(2);
    file1->setMaterial(testMaterial);
    file2->setRotateZ(15.0);
    this->_objects.append(file2);
    QObject::connect(file2, SIGNAL(redraw()), this, SLOT(updateGL()));

    LightObject* light = new LightObject();
    light->setScale(0.75);
    light->setPosition(QVector3D(3, 0, -22));
    this->_objects.append(light);
    QObject::connect(light, SIGNAL(redraw()), this, SLOT(updateGL()));

    this->nextDepth = -23;

    this->camera = QVector3D(0,0,0);

    this->selectedIndex = 2;
}

GLint loc;

void GLWidget::initializeGL()
{
    const GLubyte* pGPU = glGetString(GL_RENDERER);
    const GLubyte* pVersion = glGetString(GL_VERSION);
    const GLubyte* pShaderVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    qDebug() << "GPU: " << QString((char*)pGPU).trimmed();
    qDebug() << "OpenGL: " << QString((char*)pVersion).trimmed();
    qDebug() << "GLSL: " << QString((char*)pShaderVersion);

    glShadeModel(GL_SMOOTH);
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

    glEnable (GL_BLEND);
       glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    //glEnable(GL_COLOR_MATERIAL);

    QGLShaderProgram* program = new QGLShaderProgram();
    this->_program = program;
    //this->_program->addShaderFromSourceFile(QGLShader::Geometry, "./shader.gsf");
    this->_program->addShaderFromSourceFile(QGLShader::Vertex, "./shader.vsf");
    this->_program->addShaderFromSourceFile(QGLShader::Fragment, "./shader.fsf");

}

void GLWidget::resizeGL(int w, int h)
{

    glViewport(0,0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f,(double)w/(double)h,5.0f,100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    if (!this->_program->isLinked()) {
        this->_program->bind();
        qDebug() << this->_program->log();
    }
    glLoadIdentity();




    for (int i = 0; i < this->_objects.size(); i++) {
        glLoadName(i+1);
        this->_objects.at(i)->draw();
    }

    //glRotatef(this->camera.x(), 0, 1, 0);
    //glRotatef(this->camera.y(), 1, 0, 0);

    GLenum errCode;
    const GLubyte *errString;
    if ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        qDebug() << "OpenGL Error: " << QString((char*)errString);
    }

}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton or event->button() == Qt::RightButton){
        this->checkClick(event);
    }
}

void GLWidget::checkClick(QMouseEvent *event)
{

    int pickMode = 1; // 0 = GL_SELECT, 1 = ray picking, 2 = color picking

    if (pickMode == 0) {
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
        //glFlush();

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
    } else if (pickMode == 1){
        // ray picking, doesn't work..

        GLdouble modelView[16];
        glGetDoublev(GL_MODELVIEW_MATRIX, modelView);

        GLdouble projection[16];
        glGetDoublev(GL_PROJECTION_MATRIX, projection);

        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        GLfloat screenHeight = viewport[2],
              screenWidth  = viewport[3],
              zNear = 0.0f,
              zFar  = 1.0f;

        qreal mouseX = event->x(),
              mouseY = screenWidth - event->y();

        GLdouble pos3D_x, pos3D_y, pos3D_z;

        QVector3D rayVector;
        gluUnProject(mouseX, mouseY, zNear,
                modelView, projection, viewport,
                &pos3D_x, &pos3D_y, &pos3D_z);

        nearPoint.setX(pos3D_x);
        nearPoint.setY(pos3D_y);
        nearPoint.setZ(pos3D_z);

        gluUnProject(mouseX, mouseY, zFar,
                modelView, projection, viewport,
                &pos3D_x, &pos3D_y, &pos3D_z);

        farPoint.setX(pos3D_x);
        farPoint.setY(pos3D_y);
        farPoint.setZ(pos3D_z);



        rayVector = farPoint - nearPoint;

        rayVector.normalize();

        //qDebug() << nearPoint << farPoint;
        //qDebug() << rayVector;


        this->updateGL();


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
    } else if (pickMode == 2) {

    }

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


