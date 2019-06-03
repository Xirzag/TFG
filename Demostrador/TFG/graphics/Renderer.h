#pragma once
#include <GL/glew.h>
#include "Model.h"

class Renderer
{
public:
	
	Renderer() = delete;

	//! Renders a class with OpenGL
	template <typename Renderable>
	void static render(Renderable &renderable) {
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, renderable.vertices);
		glNormalPointer(GL_FLOAT, 0, renderable.normal);
		glTexCoordPointer(2, GL_FLOAT, 0, renderable.textCoord);

		glDrawElements(GL_TRIANGLES, 3 * renderable.n_triangles, GL_UNSIGNED_INT, renderable.index);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	};
	
	//! Renders a model with OpenGL
	void static render(Model &model) {
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		for (auto& mesh : model.meshes) {

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

};

