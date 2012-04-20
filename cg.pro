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
    textobject.cpp \
    fileobject.cpp \
    lightobject.cpp \
    material.cpp \
    material/materialtest.cpp \
    light.cpp
HEADERS += glwidget.h \
    qlobject.h \
    mainwindow.h \
    textobject.h \
    fileobject.h \
    lightobject.h \
    material.h \
    material/materialtest.h \
    light.h
FORMS += mainwindow.ui
OTHER_FILES += shader.vsf \
    shader.g.glsl \
    shader.f.glsl \
    shader.v.glsl \
    shadow.v.glsl \
    shadow.f.glsl \
    blur.f.glsl \
    blur.v.glsl
