#ifndef GRIDNSTUFF_H
#define GRIDNSTUFF_H

#include "qlobject.h"

class GridNStuff : public QLObject
{
    Q_OBJECT
public:
    explicit GridNStuff(QObject *parent = 0);

    void drawPixel(int xin, int yin);
    void drawPixel(int xin, int yin, float color);
    void drawPixel(int xin, int yin, int zin);
    void drawPixel(int xin, int yin, int zin, float color);


    void gridPlane();

    void lineDDA(QVector3D start, QVector3D end);
    void lineDDA(QVector2D start, QVector2D end);
    void lineAA(QVector3D start, QVector3D end);

    void circleMid(QVector2D mid, int radius);
signals:
    
public slots:
    
};

#endif // GRIDNSTUFF_H
