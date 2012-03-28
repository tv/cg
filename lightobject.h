#ifndef LIGHTOBJECT_H
#define LIGHTOBJECT_H

#include "qlobject.h"

class LightObject : public QLObject
{
Q_OBJECT
public:
    explicit LightObject(QObject *parent = 0);
    void draw();

signals:

public slots:

};

#endif // LIGHTOBJECT_H
