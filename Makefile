#############################################################################
# Makefile for building: hw1
# Generated by qmake (2.01a) (Qt 4.7.4) on: Tue Mar 20 18:00:42 2012
# Project:  hw1.pro
# Template: app
# Command: /usr/bin/qmake-qt4 -spec /usr/share/qt4/mkspecs/linux-g++-64 CONFIG+=debug -o Makefile hw1.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_WEBKIT -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -m64 -pipe -g -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -m64 -pipe -g -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++-64 -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtOpenGL -I/usr/include/qt4 -I/usr/X11R6/include -I. -I.
LINK          = g++
LFLAGS        = -m64
LIBS          = $(SUBLIBS)  -L/usr/lib -L/usr/X11R6/lib64 -lQtOpenGL -lQtGui -lQtCore -lGLU -lGL -lpthread 
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake-qt4
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = main.cpp \
		glwidget.cpp \
		qlobject.cpp \
		flagobject.cpp \
		mainwindow.cpp \
		textobject.cpp moc_glwidget.cpp \
		moc_qlobject.cpp \
		moc_flagobject.cpp \
		moc_mainwindow.cpp \
		moc_textobject.cpp
OBJECTS       = main.o \
		glwidget.o \
		qlobject.o \
		flagobject.o \
		mainwindow.o \
		textobject.o \
		moc_glwidget.o \
		moc_qlobject.o \
		moc_flagobject.o \
		moc_mainwindow.o \
		moc_textobject.o
DIST          = /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/opengl.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		hw1.pro
QMAKE_TARGET  = hw1
DESTDIR       = 
TARGET        = hw1

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET): ui_mainwindow.h $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: hw1.pro  /usr/share/qt4/mkspecs/linux-g++-64/qmake.conf /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/opengl.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		/usr/lib/libQtOpenGL.prl \
		/usr/lib/libQtGui.prl \
		/usr/lib/libQtCore.prl
	$(QMAKE) -spec /usr/share/qt4/mkspecs/linux-g++-64 CONFIG+=debug -o Makefile hw1.pro
/usr/share/qt4/mkspecs/common/g++.conf:
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/debug.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/opengl.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
/usr/lib/libQtOpenGL.prl:
/usr/lib/libQtGui.prl:
/usr/lib/libQtCore.prl:
qmake:  FORCE
	@$(QMAKE) -spec /usr/share/qt4/mkspecs/linux-g++-64 CONFIG+=debug -o Makefile hw1.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/hw11.0.0 || $(MKDIR) .tmp/hw11.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/hw11.0.0/ && $(COPY_FILE) --parents glwidget.h qlobject.h flagobject.h mainwindow.h textobject.h .tmp/hw11.0.0/ && $(COPY_FILE) --parents main.cpp glwidget.cpp qlobject.cpp flagobject.cpp mainwindow.cpp textobject.cpp .tmp/hw11.0.0/ && $(COPY_FILE) --parents mainwindow.ui .tmp/hw11.0.0/ && (cd `dirname .tmp/hw11.0.0` && $(TAR) hw11.0.0.tar hw11.0.0 && $(COMPRESS) hw11.0.0.tar) && $(MOVE) `dirname .tmp/hw11.0.0`/hw11.0.0.tar.gz . && $(DEL_FILE) -r .tmp/hw11.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: moc_glwidget.cpp moc_qlobject.cpp moc_flagobject.cpp moc_mainwindow.cpp moc_textobject.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_glwidget.cpp moc_qlobject.cpp moc_flagobject.cpp moc_mainwindow.cpp moc_textobject.cpp
moc_glwidget.cpp: qlobject.h \
		textobject.h \
		flagobject.h \
		glwidget.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) glwidget.h -o moc_glwidget.cpp

moc_qlobject.cpp: qlobject.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) qlobject.h -o moc_qlobject.cpp

moc_flagobject.cpp: qlobject.h \
		flagobject.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) flagobject.h -o moc_flagobject.cpp

moc_mainwindow.cpp: glwidget.h \
		qlobject.h \
		textobject.h \
		flagobject.h \
		mainwindow.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) mainwindow.h -o moc_mainwindow.cpp

moc_textobject.cpp: textobject.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) textobject.h -o moc_textobject.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all: ui_mainwindow.h
compiler_uic_clean:
	-$(DEL_FILE) ui_mainwindow.h
ui_mainwindow.h: mainwindow.ui
	/usr/bin/uic-qt4 mainwindow.ui -o ui_mainwindow.h

compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean compiler_uic_clean 

####### Compile

main.o: main.cpp glwidget.h \
		qlobject.h \
		textobject.h \
		flagobject.h \
		mainwindow.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o main.cpp

glwidget.o: glwidget.cpp glwidget.h \
		qlobject.h \
		textobject.h \
		flagobject.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o glwidget.o glwidget.cpp

qlobject.o: qlobject.cpp qlobject.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o qlobject.o qlobject.cpp

flagobject.o: flagobject.cpp flagobject.h \
		qlobject.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o flagobject.o flagobject.cpp

mainwindow.o: mainwindow.cpp mainwindow.h \
		glwidget.h \
		qlobject.h \
		textobject.h \
		flagobject.h \
		ui_mainwindow.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o mainwindow.o mainwindow.cpp

textobject.o: textobject.cpp textobject.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o textobject.o textobject.cpp

moc_glwidget.o: moc_glwidget.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_glwidget.o moc_glwidget.cpp

moc_qlobject.o: moc_qlobject.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_qlobject.o moc_qlobject.cpp

moc_flagobject.o: moc_flagobject.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_flagobject.o moc_flagobject.cpp

moc_mainwindow.o: moc_mainwindow.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_mainwindow.o moc_mainwindow.cpp

moc_textobject.o: moc_textobject.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_textobject.o moc_textobject.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

