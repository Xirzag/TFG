#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"

#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

class Model
{
public:

	Model(string const &path, bool gamma = false);

	//! draws the model, and thus all its meshes
	void render();

	//! Meshes of the model
	vector<Mesh> meshes;
private:
	
	vector<Texture> textures_loaded;
	string directory;
	bool gammaCorrection;

	void load_model(string const &path);
	
	void process_node(aiNode *node, const aiScene *scene);
	Mesh process_mesh(aiMesh *mesh, const aiScene *scene);
	
	vector<Texture> load_material_textures(aiMaterial *mat);

	glm::vec3 convert(aiVector3D &v);
	std::string convert(aiString &str);

};

