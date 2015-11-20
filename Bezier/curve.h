#ifndef BEZIER_CURVE_H_
#define BEZIER_CURVE_H_

#include <stdlib.h>
#include <GL/glut.h>


typedef struct{
	GLdouble x;
	GLdouble y;
} Point;

typedef struct{
	Point *body;
	int points;
	int reserved;
}Curve;

void initCurve(Curve *curve);

Curve* newCurve(int nPoints);

void clearCurve(Curve *curve);

void resetCurve(Curve *curve);

void addPointToTriangle();

void subPoint();

void addPoint(Curve *curve, Point point);

void deletePoint(Curve *curve, int point);

void permuteCurve(Curve *orig, Curve* dest);

//Retorna um ponto da curva de bezier, a partir de t e pontos de controle, usando recursão(Casteljau)
Point deCasteljau(Curve *curve, GLdouble t);

#endif
