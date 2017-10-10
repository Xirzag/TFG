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

class StereoVision
{
public:	
	static void DisplayFunc(std::function<void(bool)> displayFunc, int width, int height);
	static void BeforeDisplay(std::function<void()> func);
	static void AfterDisplay(std::function<void()> func);

private:
	static void init(int width, int height);
	static void initRenderBuffer(unsigned int &frameBuffer, unsigned int &textureColorBuffer);
	static void drawFunc();
	static void setOrtoProjection();

	static std::function<void(bool)> displayFunc;
	static std::function<void()> beforeFunc, afterFunc;
	static unsigned int rightFramebuffer;
	static unsigned int rightTextureColorbuffer;
	static unsigned int leftFramebuffer;
	static unsigned int leftTextureColorbuffer;
	static int width, height;

};

