#include <stdio.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>

#include <GL/glew.h>
#include "demo/SpaceDemo.h"
#include "graphics\GlInitializer.h"

void keyCallback(unsigned char tecla, int x, int y);

int main(int argc, char *argv[]) {

	std::string ip = "192.168.1.100";
	int port = 27000;
	int screen = 2;

	if (argc == 2) ip = argv[1];
	if (argc == 3) port = std::atoi(argv[2]);
	if (argc == 4) screen = std::atoi(argv[3]);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutCreateWindow("SpaceDemo");
	FullScreen::set_on_screen(screen, "SpaceDemo");

	init_GL();

	glutKeyboardFunc(keyCallback);

	NetReceiver reader(ip, port);
	reader.start();

	
	SpaceDemo demo(&reader);
	glutMainLoop();
	
	return 0;
}

void keyCallback(unsigned char tecla, int x, int y) {
	if (tecla == 'q') {
		glutExit();
	}
}
