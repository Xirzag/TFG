#include <stdio.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>

#include <GL/glew.h>
#include "demo/PlanetsDemo.h"
#include "graphics\GlInitializer.h"


void mousePositionCallback(int x, int y);
void keyCallback(unsigned char tecla, int x, int y);




//main
int main(int argc, char *argv[]) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutCreateWindow("TFG");

	FullScreen::set_on_window(2, "TFG");

	init_GL();
	
	const int WINDOW_WIDTH = glutGet(GLUT_WINDOW_WIDTH);
	const int WINDOW_HEIGHT  = glutGet(GLUT_WINDOW_HEIGHT);

	//glutReshapeFunc(ReshapeSize);
	glutMotionFunc(mousePositionCallback);
	glutKeyboardFunc(keyCallback);
	
	NetReader reader("192.168.1.100", "27000");
	reader.start();

	PlanetDemo demo(&reader);
	
	StereoVision::DisplayFunc(std::bind(&PlanetDemo::render, &demo, std::placeholders::_1), WINDOW_WIDTH, WINDOW_HEIGHT);
	StereoVision::BeforeDisplay(std::bind(&PlanetDemo::beforeRender, &demo));

	glutMainLoop();

	return 0;
}


void mousePositionCallback(int x, int y) {
	
}


void keyCallback(unsigned char tecla, int x, int y) {
	if (tecla == 'q') {
		glutExit();
	}
}
