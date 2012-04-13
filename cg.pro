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
    flagobject.cpp \
    mainwindow.cpp \
    textobject.cpp \
    fileobject.cpp \
    lightobject.cpp \
    material.cpp \
    material/materialtest.cpp
HEADERS += glwidget.h \
    qlobject.h \
    flagobject.h \
    mainwindow.h \
    textobject.h \
    fileobject.h \
    lightobject.h \
    material.h \
    material/materialtest.h
FORMS += mainwindow.ui
OTHER_FILES += shader.vsf \
    shader.fsf \
    backwall.obj
