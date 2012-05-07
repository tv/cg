# -------------------------------------------------
# Project created by QtCreator 2012-03-15T23:34:11
# -------------------------------------------------
# QT       -= gui
QT += opengl
TARGET = cg
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    glwidget.cpp \
    qlobject.cpp \
    mainwindow.cpp \
    fileobject.cpp \
    material.cpp \
    light.cpp \
    camera.cpp \
    gridnstuff.cpp
HEADERS += glwidget.h \
    qlobject.h \
    mainwindow.h \
    fileobject.h \
    material.h \
    light.h \
    camera.h \
    gridnstuff.h
FORMS += mainwindow.ui
OTHER_FILES += shader.vsf \
    shader.g.glsl \
    shader.f.glsl \
    shader.v.glsl \
    shadow.v.glsl \
    shadow.f.glsl \
    blur.f.glsl \
    blur.v.glsl \
    shaders/shadow.v.glsl \
    shaders/shadow.f.glsl \
    shaders/shader.v.glsl \
    shaders/shader.g.glsl \
    shaders/shader.f.glsl
