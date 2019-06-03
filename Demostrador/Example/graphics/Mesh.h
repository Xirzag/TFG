#pragma once

#include <assimp/Importer.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Texture.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;


struct Mesh {

	vector<glm::vec3> positions;
	vector<glm::vec3> normals;
	vector<glm::vec2> texCoords;
	vector<unsigned int> indices;
	vector<Texture> textures;
	unsigned int VAO;

	Mesh(vector<glm::vec3> positions, vector<glm::vec3> normals, vector<glm::vec2> texCoords, vector<unsigned int> indices, vector<Texture> textures)
	{
		this->positions = positions;
		this->normals = normals;
		this->texCoords = texCoords;
		this->indices = indices;
		this->textures = textures;
	}

};