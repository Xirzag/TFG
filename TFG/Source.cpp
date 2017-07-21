#include <stdio.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>

#include <GL\glew.h>
#include <GL\freeglut.h>

#include "Esfera.h"
#include "Rings.h"
#include "getbmp.h"
#include "StereoVision.h"


// Espacio para las variables globales de la ventana
float gl_fovy = 60.0f;
float gl_fovy_min = 1.0f, gl_fovy_max = 120.0f, gl_fovy_paso = 1.0f;
float gl_cerca = 1.0f, gl_lejos = 100.0f;
float gl_center = 5.0f; // centro donde pondremos los objetos

						// Espacio para otras variables globales
double theta = M_PI_2, phi = 0.0;
double radio = 10.0;
bool showRings = true;
Esfera esfera(1.0f, 500, 500);
Esfera sky(gl_lejos / 2.f, 500, 500);
Rings ring(4., 2.5f);
GLuint ringTexture, texturaAMostrar, skyTexture;



// Espacio para la declaración de funciones
void InitGlew();
void InitGL();
void Display(bool isRight);
void ReshapeSize(int ancho, int alto);
void PosicionRaton(int x, int y);
void RuedaRaton(int rueda, int sentido, int x, int y);
void ControlTecla(unsigned char tecla, int x, int y);
void setOrtoProjection();
void loadPlanets();
void DrawSphere(Esfera & esfera);
GLuint setTexture(char *filename);

struct Texture {
	std::string name;
	GLuint id = 0;
	Texture() { }

	Texture(std::string name) : name(name)
	{
		id = setTexture((char*)name.c_str());
	}
};

std::vector<Texture> planets;
void init() {
	glClearColor(0, 0, 0, 0);

	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, 1, 0);*/

}


//main
int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	const int WINDOW_ANCHO = glutGet(GLUT_SCREEN_WIDTH);
	const int WINDOW_ALTO  = glutGet(GLUT_SCREEN_HEIGHT);
	//glutInitWindowPosition(100, 100);
	//glutInitWindowSize(WINDOW_ANCHO, WINDOW_ALTO);
	//glutFullScreen();
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	//glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);

	glutCreateWindow("OpenGL Practica 12_2");
	glutFullScreen();
	InitGlew(); // despues de crear la primera ventana
	InitGL();

	

	//glutReshapeFunc(ReshapeSize);
	glutMotionFunc(PosicionRaton);
	glutMouseWheelFunc(RuedaRaton);
	glutKeyboardFunc(ControlTecla);

	loadPlanets();
	
	StereoVision::DisplayFunc(Display, WINDOW_ANCHO, WINDOW_ALTO);

	glutMainLoop();

	return 0;
}

void InitGlew() {

	// para poder utilizar trasnparentemente todas las extensiones de OpenGL

	GLenum glew_init = glewInit();
	if (glew_init != GLEW_OK)
	{
		// Problem: glewInit failed, something is seriously wrong. 
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_init));
	}
	else
		fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
}

void setViewport() {
	int ancho = glutGet(GLUT_WINDOW_WIDTH);
	int alto = glutGet(GLUT_WINDOW_HEIGHT);
	glViewport(0, 0, ancho, alto); // utiliza la totalidad de la ventana
}

void setProjection() {
	int ancho = glutGet(GLUT_WINDOW_WIDTH);
	int alto = glutGet(GLUT_WINDOW_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(gl_fovy, (float)ancho / (float)alto, gl_cerca, gl_lejos);
}

void setOrtoProjection() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glViewport(0, 0, 1, 1);
	glOrtho(-1, 1, -1, 1, 1, 0);
}

void setModelView() {

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void setLight0() {


	float ambient[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float diffuse[4] = { 1.0f,1.0f,1.0f,1.0f };
	float specular[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	float position[4] = { 1.0f, -1.0f, 1.0f, 1.0f };
	float direction[4] = { -1.0f, -1.0f, -1.0f, 1.0f };
	float cut_off = 180.0f;

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direction);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, cut_off);

	glEnable(GL_LIGHT0);
}

GLuint setTexture(char *filename) {

	// lectura de la imagen en un buffer
	BitMapFile *imagen = getbmp(filename);
	int ancho = imagen->sizeX;
	int alto = imagen->sizeY;

	// habilitar la textura 
	glEnable(GL_TEXTURE_2D);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// aunque en nuestro caso las coordenadas (s,t) estan ajustadas a [0,1]
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ancho, alto, 0, GL_RGBA, GL_UNSIGNED_BYTE, imagen->data);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}

void InitGL() {

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	// para cambiar el criterio de lo que se ve. 
	// LESS es el de defecto: se ve lo que tiene menor profundidad 
	glEnable(GL_BLEND);
	glDepthFunc(GL_LESS);
	// TO DO

	setViewport();
	setProjection();

	glEnable(GL_LIGHTING);
	//glShadeModel(GL_FLAT);
	glShadeModel(GL_SMOOTH);

	setLight0();

}

// cambio de dimensiones de la ventana, manteniendo la dimension de los objetos
void ReshapeSize(int ancho, int alto) {

	setViewport();
	setProjection();
	glutPostRedisplay();
}

int lastX = 0, lastY = 0;

void PosicionRaton(int x, int y) {

	int ancho = glutGet(GLUT_WINDOW_WIDTH);
	int alto = glutGet(GLUT_WINDOW_HEIGHT);

	theta = M_PI*(double)y / (double)alto;
	phi = -M_PI + 2 * M_PI * (double)x / (double)ancho;
	glutPostRedisplay();
}


void ControlTecla(unsigned char tecla, int x, int y) {
	static size_t pos = 1;
	if (tecla == 'a') {
		texturaAMostrar = planets[pos++].id;
		glutPostRedisplay();
		if (pos >= planets.size()) pos = 0;
	}
	else if (tecla == 's') {
		glutPostRedisplay();
		showRings = !showRings;
	}
	else if (tecla == 'q') {
		glutExit();
	}
}

void RuedaRaton(int rueda, int sentido, int x, int y) {

	gl_fovy -= gl_fovy_paso*(float)sentido;
	if (gl_fovy < gl_fovy_min) gl_fovy = gl_fovy_min;
	if (gl_fovy > gl_fovy_max) gl_fovy = gl_fovy_max;
	setProjection();
	glutPostRedisplay();
}

void loadPlanets() {
	planets.emplace_back("mercurymap.bmp");
	planets.emplace_back("venusmap.bmp");
	planets.emplace_back("earth_high.bmp");
	planets.emplace_back("moonmap4k.bmp");
	planets.emplace_back("marsmap.bmp");
	planets.emplace_back("jupiter.bmp");
	planets.emplace_back("saturnmap.bmp");
	planets.emplace_back("neptunemap.bmp");
	planets.emplace_back("uranusmap.bmp");
	planets.emplace_back("pluto.bmp");

	ringTexture = setTexture("saturnringmap.bmp");
	skyTexture = setTexture("Space.bmp");

	texturaAMostrar = planets[0].id;

}

void setMaterial() {

	/* 0.2f, 0.2f, 0.2f, 1.0f */
	/* 1.0f, 1.0f, 1.0f, 1.0f */
	/* 0.7f, 0.7f, 0.7f, 1.0f */

	const GLfloat ambient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const GLfloat diffuse[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
	const GLfloat specular[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const GLfloat shininess = 100.0f;


	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);

}

void DrawSphere(Esfera &esfera) {
	glPushMatrix();

	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // eje z arriba

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, esfera.vertices);
	glNormalPointer(GL_FLOAT, 0, esfera.normal);
	glTexCoordPointer(2, GL_FLOAT, 0, esfera.textCoord);

	glDrawElements(GL_TRIANGLES, 3 * esfera.n_triangulos, GL_UNSIGNED_INT, esfera.index);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glPopMatrix();

}

// función de gestion de ventana
void Display(bool isRight) {

	float offset = isRight? 3 : -3;

	setProjection();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // borra todo lo existente en el framebuffer
	setModelView();

	glPushMatrix();

	// poicionamos el centro de coordenadas 
	glTranslatef(offset, 0.0f, -gl_center);

	// el eje cenital es el Y
	float x = (float)(radio*sin(theta)*sin(phi));
	float y = (float)(radio*cos(theta));
	float z = (float)(radio*sin(theta)*cos(phi));

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	gluLookAt(x, y, z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f); // mira al (0,0,0)

	glBlendFunc(GL_ONE, GL_ZERO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, skyTexture);

	glDisable(GL_LIGHTING);

	DrawSphere(sky);

	glEnable(GL_LIGHTING);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texturaAMostrar);

	float position[4] = { -10.0f, 0.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	setMaterial();

	DrawSphere(esfera);

	if (showRings) {

		glBlendColor(0, 0, 0, 0.7f);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_CONSTANT_ALPHA);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ringTexture);

		glRotatef(10.0f, 0.0f, 0.0f, 1.0f); // eje z arriba

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, ring.vertices);
		glNormalPointer(GL_FLOAT, 0, ring.normal);
		glTexCoordPointer(2, GL_FLOAT, 0, ring.textCoord);

		glDrawElements(GL_TRIANGLES, 3 * ring.n_triangulos, GL_UNSIGNED_INT, ring.index);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	}

	glPopMatrix();

}
