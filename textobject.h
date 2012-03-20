#ifndef TEXTOBJECT_H
#define TEXTOBJECT_H

#include "qlobject.h"
#include "glwidget.h"

class TextObject : public QLObject
{
Q_OBJECT
public:
    explicit TextObject(QObject *parent = 0);

    void draw();

    QFont font;
signals:

public slots:

};

#endif // TEXTOBJECT_H
