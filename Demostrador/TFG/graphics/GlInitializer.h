#pragma once
#include <iostream>
#include <GL\glew.h>

//! Inits the OpenGL context
void init_GL() {
	GLenum glew_init = glewInit();
	if (glew_init != GLEW_OK)
	{
		std::cerr << "Error: " << glewGetErrorString(glew_init);
		exit(-1);
	}
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	glEnable(GL_BLEND);
	
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);

};