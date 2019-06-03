#include <iostream>
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

const std::string Texture::defaultFolder = "resources/img";

Texture::Texture() { };

Texture::Texture(std::string filename, std::string folder) {
	
	path = folder.empty()? filename : folder + '/' + filename;

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);

	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &textureId);

	if (data)
	{

		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
				

		glBindTexture(GL_TEXTURE_2D, textureId);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, format, GL_UNSIGNED_BYTE, data);


		stbi_image_free(data);

		glBindTexture(GL_TEXTURE_2D, 0);

	}
	else {
		throw "Image " + path + " not found";
	}

}

GLuint Texture::get_id()
{
	return textureId;
}

std::string Texture::get_path()
{
	return path;
}
