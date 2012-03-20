#include "textobject.h"

TextObject::TextObject(QObject *parent) :
    QLObject(parent)
{
    this->ignoreClickEvent = true;
    this->font = QFont("Helvetica", 10, QFont::Bold);
}

void TextObject::draw()
{
    glColor4f(1.0, 0.0, 0.0, 1.0);
    GLWidget* parent = (GLWidget*)this->parent();

    parent->renderText(10, 20,  QString("Tuomas Vähänen - 1962817") , this->font);
    parent->renderText(10, 40,  QString("n = new flag") , this->font);
    parent->renderText(10, 60,  QString("d = delete flag"), this->font);
    parent->renderText(10, 80,  QString("arrows = rotate selected") , this->font);
    parent->renderText(10, 100, QString("rmouse = move selected") , this->font);
    parent->renderText(10, 120, QString("lmouse = change type") , this->font);
    parent->renderText(10, 140, QString("mmouse = rotate selected") , this->font);
}
