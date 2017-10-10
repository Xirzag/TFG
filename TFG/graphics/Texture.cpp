#include "Texture.h"

Texture::Texture() { };

Texture::Texture(std::string filename) {
	// lectura de la imagen en un buffer
	BitMapFile *imagen = getbmp("resources/img/" + filename);
	int width = imagen->sizeX;
	int alto = imagen->sizeY;

	// habilitar la textura 
	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	// aunque en nuestro caso las coordenadas (s,t) estan ajustadas a [0,1]
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, alto, 0, GL_RGBA, GL_UNSIGNED_BYTE, imagen->data);

	glBindTexture(GL_TEXTURE_2D, 0);

};

GLuint Texture::getId()
{
	return textureId;
}
