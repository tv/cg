#ifndef FILEOBJECT_H
#define FILEOBJECT_H

#include "qlobject.h"
#include <QTextStream>

class FileObject : public QLObject
{
Q_OBJECT
public:
    explicit FileObject(QObject *parent = 0);

    void readFile(QString path);
protected:
    int stringToInt(QString str, bool *ok);
    float stringToFloat(QString str, bool *ok);
signals:

public slots:

};

#endif // FILEOBJECT_H
