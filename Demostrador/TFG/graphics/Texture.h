#pragma once
#include <string>
#include <GL\glew.h>

//! Loads a texture into OpenGL
class Texture {
	static const std::string defaultFolder;
	
	GLuint textureId;
	std::string path;
public:
	Texture();
	Texture(std::string name, std::string folder = defaultFolder);
	
	//! Returns the OpenGL texture id
	GLuint get_id();

	//! Returns the texture path
	std::string get_path();
};