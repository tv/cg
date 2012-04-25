#include "light.h"

Light::Light(QObject *parent) :
    QObject(parent)
{
    this->diffuse = QVector4D(1.0, 1.0, 1.0, 1.0);
    this->specular = QVector4D(1.0, 1.0, 1.0, 1.0);

    this->attenuations = QVector3D(1.0, 0.01, 0.0);
    this->spotCutOff = 45.0f;
    this->spotExponent = 1.0;

    this->initializedFBO = false;
    this->initializingFBO = false;
}

void Light::injectToShader(QGLShaderProgram *p, int key) {
    Camera* cam = this->camera;

    QString name = QString("in_light%1_shadow").arg(key);
    char *ptr = name.toAscii().data();

    glActiveTexture(GL_TEXTURE0+key);
    glBindTexture(GL_TEXTURE_2D, this->depthTextureId);

    p->setUniformValue(ptr, key);

    GLenum errCode;
    const GLubyte *errString;
    GLenum FBOstatus;
    if ((errCode = glGetError()) != GL_NO_ERROR) {

        qDebug() << "Light::inject after texture OpenGL Error: " << errCode;
    }


    GLfloat mat[21] = {
        cam->position.x(), cam->position.y(), cam->position.z(), cam->position.w(),
        this->diffuse.x(), this->diffuse.y(), this->diffuse.z(), this->diffuse.w(),
        this->specular.x(), this->specular.y(), this->specular.z(), this->specular.w(),

        this->attenuations.x(), this->attenuations.y(), this->attenuations.z(),
        this->spotCutOff,
        this->spotExponent,

        cam->direction.x(), cam->direction.y(), cam->direction.z(),
    };

    name = QString("in_light%1").arg(key);
    ptr = name.toAscii().data();
    p->setUniformValueArray(ptr, mat, 21, 1);

    this->camera->injectToShader(p, name);
}

void Light::setCamera(Camera* cam)
{
    this->camera = cam;
}

void Light::updateProjection(double aspect)
{
    //this->camera->setPerspective(45.0f, aspect, 0.1, 100.0);
    this->camera->setPerspective(this->spotCutOff, aspect, 0.1, 100.0);
}

void Light::initializeShadowFBO(QSize screen)
{
    if (this->initializingFBO) return;

    this->initializingFBO = true;
    int mapWidth = screen.width()  * 0.5,
        mapHeight = screen.height() * 0.5;


    GLenum errCode;
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


    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        mapWidth, mapHeight, 0,
        GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthTextureId, 0);

    // Color texture
    glGenTextures(1, &this->colorTextureId);
    glBindTexture(GL_TEXTURE_2D, this->colorTextureId);

    if ((errCode = glGetError()) != GL_NO_ERROR) {

        qDebug() << "initialize before parametri color OpenGL Error: " << errCode;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    if ((errCode = glGetError()) != GL_NO_ERROR) {

        qDebug() << "initialize after color parametri OpenGL Error: " << errCode;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F_ARB, mapWidth, mapHeight, 0, GL_RGB, GL_FLOAT, 0);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,0);

    if ((errCode = glGetError()) != GL_NO_ERROR) {

        qDebug() << "initialize after color OpenGL Error: " << errCode;
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, this->colorTextureId, 0);

    // Fbo for those
    FBOstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (FBOstatus) {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            qDebug() << "shadow success!";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
            qDebug() << " Attachment Point Unconnected";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            qDebug() << " Error: Missing Attachment";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            qDebug() << " Error: Dimensions do not match";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            qDebug() << " Error: Formats";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            qDebug() << " Error: Draw Buffer";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            qDebug() << " Error: Read Buffer";
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            qDebug() << " Error: Unsupported Framebuffer Configuration";
            break;
        default:
            qDebug() << " Error: Unkown Framebuffer Object Failure";
            break;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    if ((errCode = glGetError()) != GL_NO_ERROR) {

        qDebug() << "initialize after unbind OpenGL Error: " << errCode;
    }




    this->initializedFBO = true;
    this->initializingFBO = false;
}

void Light::bindFBO() {
    glBindFramebuffer(GL_FRAMEBUFFER, this->fboId);
    GLenum errCode;
    const GLubyte *errString;
    GLenum FBOstatus;
    if ((errCode = glGetError()) != GL_NO_ERROR) {

        qDebug() << "initialize after unbind OpenGL Error: " << errCode;
    }
}

void Light::releaseFBO() {
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GLenum errCode;
    const GLubyte *errString;
    GLenum FBOstatus;
    if ((errCode = glGetError()) != GL_NO_ERROR) {

        qDebug() << "initialize after unbind OpenGL Error: " << errCode;
    }
}

void Light::bindDebugShadowMap()
{
    glActiveTextureARB(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,this->depthTextureId);
}

void Light::animate()
{
    //this->angle += 5.0f;

    //this->direction.setX(sin(this->angle/180));
    //this->direction.setZ(cos(this->angle/180));

    //this->direction.normalize();
}
