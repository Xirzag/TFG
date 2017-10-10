#pragma once
#include <iostream>
#include <GL\glew.h>

void init_GL() {
	GLenum glew_init = glewInit();
	if (glew_init != GLEW_OK)
	{
		std::cerr << "Error: " << glewGetErrorString(glew_init);
		exit(-1);
	}
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	// para cambiar el criterio de lo que se ve. 
	// LESS es el de defecto: se ve lo que tiene menor profundidad 
	glEnable(GL_BLEND);
	glDepthFunc(GL_LESS);
	// TO DO


	glEnable(GL_LIGHTING);
	//glShadeModel(GL_FLAT);
	glShadeModel(GL_SMOOTH);

};