#include <QtGui/QApplication>


#include "glwidget.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GLWidget window;
    window.resize(800,600);
    
    window.show();

    return a.exec();
}
