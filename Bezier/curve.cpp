#include "curve.h"


void initCurve(Curve *curve){
	curve->points = 0;
	curve->reserved = 10;
	curve->body = (Point*)malloc(sizeof(Point)*curve->reserved);
}

Curve* newCurve(int nPoints){
	Curve *curve;
	curve = (Curve*)malloc(sizeof(Curve));
	curve->points = 0;
	curve->reserved = nPoints;
	curve->body = (Point*)malloc(sizeof(Point)*nPoints);
	return curve;
}

void clearCurve(Curve *curve){
	if (curve->reserved >0){
		free(curve->body);
	}
	free(curve);
}

void resetCurve(Curve *curve){
	curve->points = 0;
	if (curve->reserved >0){
		free(curve->body);
	}
	curve->reserved = 0;
}


void addPointToTriangle();
void subPoint();
void addPoint(Curve *curve, Point point){
	if (curve->points < curve->reserved){
		curve->body[curve->points] = point;
		curve->points++;
	}
	else{
		curve->reserved += 10;
		curve->body = (Point*)realloc(curve->body, sizeof(Point)*(curve->reserved));
		curve->body[curve->points++] = point;
	}
	addPointToTriangle();
}

void deletePoint(Curve *curve, int point){
	for (int i = point; i<curve->points; i++){
		curve->body[i] = curve->body[i + 1];
	}
	subPoint();
	curve->points--;
}

void permuteCurve(Curve *orig, Curve* dest){
	if (dest->reserved < orig->reserved){
		dest->reserved = orig->reserved;
		dest->body = (Point*)realloc(dest->body, sizeof(Point)*(dest->reserved));
	}

	dest->points = orig->points;
	for (int i = 0; i<orig->points; i += 2){
		dest->body[i] = orig->body[i + 1];
		dest->body[i + 1] = orig->body[i];
	}
	if (orig->points % 2){
		dest->body[orig->points - 1] = orig->body[orig->points - 1];
	}
}

//Retorna um ponto da curva de bezier, a partir de t e pontos de controle, usando recursão(Casteljau)
Point deCasteljau(Curve *curve, GLdouble t){
	Point point;

	if (curve->points == 1){
		point = curve->body[0];
	}
	else{
		Curve *nextCurve = newCurve(curve->points - 1);
		nextCurve->points = curve->points - 1;
		for (int i = 0; i<curve->points - 1; i++){
			nextCurve->body[i].x = curve->body[i].x + ((curve->body[i + 1].x - curve->body[i].x)*t);
			nextCurve->body[i].y = curve->body[i].y + ((curve->body[i + 1].y - curve->body[i].y)*t);
		}
		point = deCasteljau(nextCurve, t);
		clearCurve(nextCurve);
	}
	return point;
}