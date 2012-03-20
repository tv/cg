#ifndef FLAGOBJECT_H
#define FLAGOBJECT_H

#include <qlobject.h>

class FlagObject : public QLObject
{
Q_OBJECT
public:
    explicit FlagObject(QObject *parent = 0);

    void draw();

    void setType(qreal type);
protected:
    virtual void _drawBase();
    virtual void _drawContent();


    qreal _type;
signals:

public slots:

};

#endif // FLAGOBJECT_H
