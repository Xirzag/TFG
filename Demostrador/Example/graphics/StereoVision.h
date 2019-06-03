#pragma once
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include <functional>

#include <GL\glew.h>
#include <GL\freeglut.h>

#include "Texture.h"

//! Static class to specify a callback to display in stereo, like glutDisplayFunc
class StereoVision
{
public:	
	//! Set a callback for rendering the scene.
	
	//! The function will be called twice, one for each part of the stereo
	//! The callback will be called setting the boolean parameter to true if it must render the right part
	static void display_func(std::function<void(bool)> displayCallback,
		int screenWidth,
		int screenHeight
	);
	//! Set a callback which will be called before the display function
	static void before_display(
		std::function<void()> functionCallback
	);
	//! Set a callback which will be called after the display function and the rendering onto the main framebuffer
	static void after_display(
		std::function<void()> functionCallback
	);

	//! Set the distortion of the lens correction shader
	static void lens_barrel_power(float power);
	//! Returns the distortion of the lens correction shader
	static float lens_barrel_power();

private:
	static void init(int width, int height);
	static void init_framebuffer(unsigned int &frameBuffer, unsigned int &textureColorBuffer);
	static void draw_function();
	static void set_orto_projection();
	static void init_shader();
	static void check_compile_errors(GLuint object, std::string type);
	static std::string read_file(std::string path);

	static std::function<void(bool)> displayFunc;
	static std::function<void()> beforeFunc, afterFunc;
	static unsigned int rightFramebuffer;
	static unsigned int rightTextureColorbuffer;
	static unsigned int leftFramebuffer;
	static unsigned int leftTextureColorbuffer;
	static unsigned int shaderId;
	static Texture grid;
	static float barrelPower;

	static int width, height;

};

