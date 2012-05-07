#include "material.h"

Material::Material()
{
    this->loadTestMaterial();
}

void Material::loadTestMaterial()
{
    this->ambient = QVector4D(0.4f, 0.4f, 0.4f, 1.0f);
    this->specular = QVector4D(0.4f, 0.4f, 0.4f, 1.0f);
    this->diffuse = QVector4D(0.8f, 0.8f, 0.8f, 1.0f);
    this->emission = QVector4D(0.8f, 0.8f, 0.8f, 1.0f);
    this->shininess = 40.0f;

    this->textureId = 0;
}

void Material::injectToShader(QGLShaderProgram* p)
{
    GLenum errCode;
    if ((errCode = glGetError()) != GL_NO_ERROR) {
        qDebug() << "Material::inject before OpenGL Error: " << errCode;
    }
    GLfloat mat[] = {
        this->ambient.x(),  this->ambient.y(),  this->ambient.z(),  this->ambient.w(),
        this->diffuse.x(),  this->diffuse.y(),  this->diffuse.z(),  this->diffuse.w(),
        this->emission.x(), this->emission.y(), this->emission.z(), this->emission.w(),
        this->specular.x(), this->specular.y(), this->specular.z(), this->specular.w(),
        this->shininess
    };

    p->setUniformValueArray("in_material", mat, 17, 1);

    if (this->textureId != 0) {
        if ((errCode = glGetError()) != GL_NO_ERROR) {
            qDebug() << "Material::inject before texture OpenGL Error: " << errCode;
        }

        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, this->textureId);
        p->setUniformValue("texture", 8);

        if ((errCode = glGetError()) != GL_NO_ERROR) {
            qDebug() << "Material::inject after texture OpenGL Error: " << errCode;
        }
    }

    if ((errCode = glGetError()) != GL_NO_ERROR) {
        qDebug() << "Material::inject after OpenGL Error: " << errCode;
    }
}

void Material::setAmbient(QVector4D in)
{
    this->ambient = in;
}

void Material::setSpecular(QVector4D in)
{
    this->specular = in;
}

void Material::setDiffuse(QVector4D in)
{
    this->diffuse = in;
}

void Material::setEmission(QVector4D in)
{
    this->emission = in;
}

void Material::setShihiness(GLfloat in)
{
    this->shininess = in;
}

void Material::setImage(QString in)
{
    QImage image(in);
    image = image.convertToFormat(QImage::Format_ARGB32);

    glGenTextures(1, &this->textureId);
    glBindTexture(GL_TEXTURE_2D, this->textureId);

    GLenum errCode;
    if ((errCode = glGetError()) != GL_NO_ERROR) {
        qDebug() << "Material::Image before OpenGL Error: " << errCode;
    }


    if ((errCode = glGetError()) != GL_NO_ERROR) {
        qDebug() << "Material::Image 1 OpenGL Error: " << errCode;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    if ((errCode = glGetError()) != GL_NO_ERROR) {
        qDebug() << "Material::Image 2 OpenGL Error: " << errCode;
    }

    glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RGBA8, image.width(),
                image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());

    qDebug() << image.width() << image.height();
    if ((errCode = glGetError()) != GL_NO_ERROR) {
        qDebug() << "Material::Image 3 OpenGL Error: " << errCode;
    }

    glGenerateMipmap(GL_TEXTURE_2D);

    if ((errCode = glGetError()) != GL_NO_ERROR) {
        qDebug() << "Material::Image 4 OpenGL Error: " << errCode;
    }

    if ((errCode = glGetError()) != GL_NO_ERROR) {
        qDebug() << "Material::Image after OpenGL Error: " << errCode;
    }
}


