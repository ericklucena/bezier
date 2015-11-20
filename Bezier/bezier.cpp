/*
Projeto 1 - Processamento Gráfico

Aluno: 
	Erick Lucena Palmeira Silva		[ELPS]
*/

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include "curve.h"

#define PI 3.14159

//Variáveis globais
Curve curve;
Curve permutedCurve;
unsigned long long **pascalTriangle;
double **power;
bool inMotion = false;
int pointInMotion;
int perfection=500;
Point scale;
Point mouse;
GLfloat width, height;
GLint view_w, view_h;

bool showCurve=true;
bool showPermutedCurve=true;
bool showCasteljau=true;
bool showPolygonal = true;
bool showPoints = true;

int triSize = 0;
int allocated = 0;

void addPointToTriangle() {
	triSize++;
	if (allocated == 0) {
		pascalTriangle = (unsigned long long **) malloc(
				sizeof(unsigned long long*)*2);
		pascalTriangle[0] = (unsigned long long *) malloc(
						sizeof(unsigned long long));
		pascalTriangle[0][0] = 1;
		pascalTriangle[0][0] = 1;
		pascalTriangle[1] = (unsigned long long *) malloc(
								sizeof(unsigned long long)*2);
		pascalTriangle[1][0] = 1;
		pascalTriangle[1][1] = 1;
		power = (double **) malloc(sizeof(double*));
		power[0] = (double*) malloc(sizeof(double)*1000);

		allocated++;
	} else if (triSize > allocated) {
		allocated++;

		power = (double **) realloc(power,sizeof(double*) * allocated);
		power[allocated -1] = (double*) malloc(sizeof(double)*1000);

		pascalTriangle = (unsigned long long **) realloc(pascalTriangle,
				sizeof(unsigned long long *) * (allocated + 1));
		pascalTriangle[allocated] = (unsigned long long*) malloc(
				sizeof(unsigned long long) * (allocated + 1));
		pascalTriangle[allocated][0] = 1;
		pascalTriangle[allocated][allocated] = 1;
		for (int i = 1; i < allocated; i++) {
			pascalTriangle[allocated][i] = pascalTriangle[allocated - 1][i]
					+ pascalTriangle[allocated - 1][i - 1];
		}
	}
}

void subPoint(){
	triSize--;
}

//Calcula as potencias quando o t ou n são modificados.
void calculatePower(){
	double t=0;
	for(int j=0;j<=perfection; j++){
		for(int i=0;i<curve.points;i++){
			if(i==0){
				power[i][j] = pow(1-t, curve.points-1-i)*pow(t, i);
			}else{
				power[i][j] = power[i-1][j]/(1-t)*t;
			}
		}
		t+=((double)1/perfection);
	}
}

//Desenha um círculo preenchido na tela.
void drawCircle(Point center, int radius){
	double angle;
	double x, y;
	int t =720;
	glBegin(GL_POLYGON);
		glColor3f(0.0,0.0,1.0);
		for(int i=0; i<t; i++){
			angle = (2+PI/t)*i;
			x = cos(angle)*radius;
			y = sin(angle)*radius;
			glVertex2d(center.x+x, center.y+y);
		}
	glEnd();
}

//Desenha um circulo com borda na tela
void drawOutlinedCircle(Point center, int radius){
	double angle;
	double x, y;
	int t =720;
	glBegin(GL_POLYGON);
		glColor3f(0.0,0.0,1.0);
		for(int i=0; i<t; i++){
			angle = (2+PI/t)*i;
			x = cos(angle)*radius;
			y = sin(angle)*radius;
			glVertex2d(center.x+x, center.y+y);
		}
	glEnd();

	t=90;
	glLineWidth(1);
	glBegin(GL_LINE_STRIP);
		glColor3f(1.0,1.0,1.0);
		for(int i=0; i<t; i++){
			angle = (2+PI/t)*i;
			x = cos(angle)*radius;
			y = sin(angle)*radius;
			glVertex2d(center.x+x, center.y+y);
		}
	glEnd();
}

//Desenha um grid no plano de fundo da tela.
void drawGrid(){
	glLineWidth(2);
	glColor3f(0.2,0.2,0.2);
	for(int i=10;i<width;i+=10){
		glBegin(GL_LINE_STRIP);
			glVertex2i(i, 0);
			glVertex2i(i, height);
		glEnd();
	}
	for(int i=10;i<height;i+=10){
		glBegin(GL_LINE_STRIP);
			glVertex2i(0, i);
			glVertex2i(width, i);
		glEnd();
	}

	glLineWidth(2);
	glColor3f(0.4,0.4,0.4);
	glBegin(GL_LINE_STRIP);
		glVertex2i(0, height/2);
		glVertex2i(width, height/2);
	glEnd();
	glBegin(GL_LINE_STRIP);
		glVertex2i(width/2, 0);
		glVertex2i(width/2, height);
	glEnd();
}

//Desenha a barra de controle do t, no canto direito da tela
void drawScale(){
	glColor3f(((double)1/(height-10))*scale.y,0,0);
	glBegin(GL_POLYGON);
		glVertex2i(width -9, height -1);
		glVertex2i(width, height -1);
		glVertex2i(width, 1);
		glVertex2i(width -9, 1);
	glEnd();
	glColor3f(1,1,1);
	glLineWidth(1);
	glBegin(GL_LINE_STRIP);
		glVertex2i(width -10, height);
		glVertex2i(width, height);
		glVertex2i(width, 1);
		glVertex2i(width -10, 1);
		glVertex2i(width -10, height);
	glEnd();
}

void drawScaleControl(){
	drawOutlinedCircle(scale, 5);
}


//Retorna um ponto da curva de bezier, a partir de t e pontos de controle, usando bernstein
Point bezier(Curve *curve,  int t){
	Point sum;
	int n = curve->points-1;
	if(n<1){
		n=0;
	}
	double aux;
	
	sum.x = 0;
	sum.y = 0;



	//Somatório
	if(curve->points){
		for(int i=0;i<=n;i++){
			aux = pascalTriangle[n][i]*power[i][t];
			sum.x += aux*curve->body[i].x; 
			sum.y += aux*curve->body[i].y;
		}
	}
	

	return sum;
}

//Desenha uma sequencia de linhas, ligando os pontos de controle
void drawLinks(){
	glLineWidth(2.0);
	glColor3f(0.0,0.4,0.0);
	glBegin(GL_LINE_STRIP);
		for(int i=0; i<curve.points;i++){
			glVertex2f(curve.body[i].x, curve.body[i].y);
		}
	glEnd();
}

void drawLinks(Curve *curve){
	glLineWidth(2.0);
	glColor3f(0.4,0.0,0.4);
	glBegin(GL_LINE_STRIP);
		for(int i=0; i<curve->points;i++){
			glVertex2f(curve->body[i].x, curve->body[i].y);
		}
	glEnd();
}

//Desenha a curva de bezier, usando bernstein
void drawBezier(){
	Point aux;
	double t = (double)1/perfection;

	glLineWidth(3.0);
	glBegin(GL_LINE_STRIP);
		for(int i=0; i<perfection; i++){
			glColor3f(1.0f, 1.0f-i*t, 1.0f-i*t);
			aux = bezier(&curve, i);
			glVertex2d(aux.x, aux.y);
		}
		glVertex2d(curve.body[curve.points-1].x, curve.body[curve.points-1].y);
	
	glEnd();
	/*
	for(int i=0; i<curve.points; i++){
		drawOutlinedCircle(curve.body[i], 6);
	}*/
	
	glLineWidth(1.0);
}

//Desenha a curva de bezier, usando casteljau
void drawBezierCasteljau(){
	Point aux;
	GLdouble t = (GLdouble)1/perfection;
	if(curve.points){
		glLineWidth(3.0);
		glBegin(GL_LINE_STRIP);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex2d(curve.body[0].x, curve.body[0].y);
			for(int i=0; i<perfection; i++){
				glColor3f(1.0f-i*t, 1.0f-i*t, 1.0f);
				aux = deCasteljau(&curve, t*i);
				//aux = animatedDeCasteljau(&curve, t*i);
				glVertex2d(aux.x, aux.y);
			}
			glVertex2d(curve.body[curve.points-1].x, curve.body[curve.points-1].y);
	
		glEnd();
	}
	/*
	for(int i=0; i<curve.points; i++){
		drawOutlinedCircle(curve.body[i], 6);
	}*/
	
	glLineWidth(1.0);
}

//Desenha a curva de bezier permutada, usando bernstein
void drawPermutedBezier(){
	permuteCurve(&curve, &permutedCurve);
	
	Point aux;
	double t = (double)1/perfection;

	glLineWidth(3.0);
	glBegin(GL_LINE_STRIP);
		for(int i=0; i<perfection; i++){
			glColor3f(1.0f-i*t, 1.0f, 1.0f-i*t);
			aux = bezier(&permutedCurve, i);
			glVertex2d(aux.x, aux.y);
		}
		glVertex2d(permutedCurve.body[permutedCurve.points-1].x, permutedCurve.body[permutedCurve.points-1].y);
	
	glEnd();
	/*
	for(int i=0; i<permutedCurve.points; i++){
		drawOutlinedCircle(permutedCurve.body[i], 6);
	}*/
	
	glLineWidth(1.0);
}

//Desenha a curva de bezier permutada, usando casteljau
void drawPermutedBezierCasteljau(){
	permuteCurve(&curve, &permutedCurve);

	Point aux;
	GLdouble t = (GLdouble)1/perfection;
	if(permutedCurve.points){
		glLineWidth(3.0);
		glBegin(GL_LINE_STRIP);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex2d(permutedCurve.body[0].x, permutedCurve.body[0].y);
			for(int i=0; i<perfection; i++){
				glColor3f(1.0f, 1.0f-i*t, 1.0f);
				aux = deCasteljau(&permutedCurve, t*i);
				//aux = animatedDeCasteljau(&permutedCurve, t*i);
				glVertex2d(aux.x, aux.y);
			}
			glVertex2d(permutedCurve.body[permutedCurve.points-1].x, permutedCurve.body[permutedCurve.points-1].y);
	
		glEnd();
	}
	/*
	for(int i=0; i<permutedCurve.points; i++){
		drawOutlinedCircle(permutedCurve.body[i], 6);
	}*/
	
	glLineWidth(1.0);
}



//Desenha os pontos de controle
void drawPoints(){
	

	for(int i=0; i<curve.points; i++){
		drawOutlinedCircle(curve.body[i], 6);
	}
	
	glLineWidth(1.0);
}



//Chamado pelo mouse, detecta se existe algum ponto de controle ou a escala no ponto clicado
void findPointToMove(){

	for(int i=0;i<curve.points;i++){
		if((mouse.x<(curve.body[i].x+6 )&& mouse.x>(curve.body[i].x-6)) && (mouse.y<(curve.body[i].y+6 )&& mouse.y>(curve.body[i].y-6))){
		pointInMotion = i;
		return;
		}
	}
	if((mouse.x<(scale.x+6 )&& mouse.x>(scale.x-6)) && (mouse.y<(scale.y+6 )&& mouse.y>(scale.y-6))){
		pointInMotion = -3;
		return;
	}


	pointInMotion = -4;
}

// Função callback chamada para fazer o desenho
void drawScene(void)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT);
	drawGrid();
	
	if(showPolygonal){
		drawLinks();
	}

	

	if(showCasteljau){
		if(showCurve){
			drawBezierCasteljau();
		}
		if(showPermutedCurve){
			drawPermutedBezierCasteljau();
		}
	}else{
		if(showCurve){
			drawBezier();
		}
		if(showPermutedCurve){
			drawPermutedBezier();
		}
	}

	if(showPoints){
		drawPoints();
	}
	drawScale();
	drawScaleControl();

	glFlush();
}

// Inicializa parâmetros
void init(void)
{   
	// Define a cor de fundo da janela de visualização como preta
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	width = 1280;
	height = 720;
	//Escala
	scale.x = 1275;
	scale.y = 715;

	//Inicia a curva
	initCurve(&curve);
	initCurve(&permutedCurve);
	//Inicia o vetor de potencias

}



// Função callback chamada quando o tamanho da janela é alterado 
void resizeWindow(GLsizei w, GLsizei h)
{ 
	// Especifica as dimens�es da Viewport
	glViewport(0, 0, w, h);
	view_w = w;
	view_h = h;                   
	width = w;
	height = h;

	scale.x = width - 5;
	scale.y = height -5;
	// Inicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D (0, width, 0, height);
}


// Função callback chamada para gerenciar eventos de teclado
void manageKeyboard(unsigned char key, int x, int y)
{
	if(key=='A' || key=='a'){
		showCurve = !showCurve;
	}

	if(key=='S' || key=='s'){
		showPermutedCurve = !showPermutedCurve;
	}

	if(key=='D' || key=='d'){
		showCasteljau = !showCasteljau;
	}

	if(key=='F' || key=='f'){
		showPolygonal = !showPolygonal;
	}
	
	if(key=='G' || key=='g'){
		showPoints = !showPoints;
	}


	glutPostRedisplay();
}

//Função que gerencia a movimentação
void handleMotion(int x, int y)
{

	if(inMotion){
		if(pointInMotion==-3){
			scale.y = height -y;

			//Impede controle sair da tela
			if(scale.y>height - 5){
				scale.y=height - 5;
			}else if(scale.y<5){
				scale.y=5;
			}
			if(scale.y>height/2){
				perfection = (scale.y-height/2)*400/(height/2 - 5)+100;
				calculatePower();
			}else{
				perfection = (scale.y-5)*(90)/((height/2-5)) +10;
				calculatePower();
			}

		}else{
			curve.body[pointInMotion].x = x;
			curve.body[pointInMotion].y = height - y;
			if(curve.body[pointInMotion].x<0){
				curve.body[pointInMotion].x = 0;
			}
			if(curve.body[pointInMotion].x>width-11){
				curve.body[pointInMotion].x = width-11;
			}
			if(curve.body[pointInMotion].y<0){
				curve.body[pointInMotion].y = 0;
			}
			if(curve.body[pointInMotion].y>height){
				curve.body[pointInMotion].y = height;
			}
		}
	}
	glutPostRedisplay();

}

// Função callback chamada para gerenciar eventos do mouse
void handleMouse(int button, int state, int x, int y)
{
	if (!inMotion && button == GLUT_LEFT_BUTTON){
		mouse.x = x;
		mouse.y = height - y;
		if (state == GLUT_DOWN) {
			findPointToMove();
			if(pointInMotion > -4){
				inMotion = true;
			}
			
		}
		
		
	}else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){
		inMotion = false;
	}else if(button == GLUT_RIGHT_BUTTON){
		if(state == GLUT_DOWN){
			//Só cria um novo ponto de controle quando o botão é solto
		}else{
			mouse.x = x;
			mouse.y = height - y;
			addPoint(&curve, mouse);
			calculatePower();
		}
	}else if(button == GLUT_MIDDLE_BUTTON && state== GLUT_UP){
		mouse.x = x;
		mouse.y = height - y;
		findPointToMove();
		if(pointInMotion>=0){
			deletePoint(&curve, pointInMotion);	
		}
		calculatePower();
	}

	glutPostRedisplay();
}

// Função callback chamada para gerenciar eventos do teclado   
// para teclas especiais, tais como F1, PgDn e Home
void especialKeys(int key, int x, int y)
{
	glutPostRedisplay();
}

// Programa Principal 
int main(int argc, char **argv) {
	glutInit(&argc, argv);
	//initPascal();
	init();
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(1280,720);
	glutCreateWindow("Bezier Curve");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resizeWindow);
	glutKeyboardFunc(manageKeyboard);
	glutMouseFunc(handleMouse);
	glutMotionFunc(handleMotion);
	glutSpecialFunc(especialKeys);     
	glutMainLoop();
}
