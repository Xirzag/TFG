#pragma once
#include <string>
#include <GL\glew.h>
#include "getbmp.h"

class Texture {
	GLuint textureId;
public:
	Texture();
	Texture(std::string name);
	GLuint getId();
};