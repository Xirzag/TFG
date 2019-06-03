#include"Model.h"

Model::Model(string const &path, bool gamma) : gammaCorrection(gamma)
{
	load_model(path);
}

// draws the model, and thus all its meshes
void Model::render()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	for (auto& mesh : meshes) {

		unsigned int numTextures = 0;
		for (auto& texture : mesh.textures)
		{
			glActiveTexture(GL_TEXTURE0 + numTextures);
			glBindTexture(GL_TEXTURE_2D, texture.get_id());
			numTextures++;
		}

		glTexCoordPointer(2, GL_FLOAT, 0, &mesh.texCoords[0]);
		glVertexPointer(3, GL_FLOAT, 0, &mesh.positions[0]);
		glNormalPointer(GL_FLOAT, 0, &mesh.normals[0]);
		glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, &mesh.indices[0]);

		while (numTextures > 0) {
			numTextures--;
			glActiveTexture(GL_TEXTURE0 + numTextures);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Model::load_model(string const &path)
{
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}
	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));

	// process ASSIMP's root node recursively
	process_node(scene->mRootNode, scene);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::process_node(aiNode *node, const aiScene *scene)
{
	// process each mesh located at the get_frame node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(process_mesh(mesh, scene));
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		process_node(node->mChildren[i], scene);
	}

}

Mesh Model::process_mesh(aiMesh *mesh, const aiScene *scene)
{
	// data to fill
	vector<glm::vec3> positions;
	vector<glm::vec3> normals;
	vector<glm::vec2> texCoords;
	vector<unsigned int> indices;
	vector<Texture> textures;

	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		
		positions.push_back( convert(mesh->mVertices[i]) );
		if (mesh->mNormals) {
			normals.push_back(convert(mesh->mNormals[i]));
		}
		else {
			normals.push_back(glm::vec3(0));
		}
		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			texCoords.push_back(glm::vec2(convert(mesh->mTextureCoords[0][i])));
		}
		else
			texCoords.push_back(glm::vec2(0));
	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	// 1. diffuse maps
	vector<Texture> diffuseMaps = load_material_textures(material);
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

	// return a mesh object created from the extracted mesh data
	return Mesh(positions, normals, texCoords, indices, textures);
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
vector<Texture> Model::load_material_textures(aiMaterial *mat)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(aiTextureType_DIFFUSE); i++)
	{
		aiString str;
		mat->GetTexture(aiTextureType_DIFFUSE, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;


		for (auto& loadedTexture : textures_loaded)
		{
			if (loadedTexture.get_path() == convert(str)) 
			{
				textures.push_back(loadedTexture);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture(str.C_Str(), this->directory);
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

glm::vec3 Model::convert(aiVector3D & v)
{
	return glm::vec3(v.x, v.y, v.z);
}

std::string Model::convert(aiString & str)
{
	return std::string(str.C_Str());
}


