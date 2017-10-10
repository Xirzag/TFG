#pragma once
#include <GL/glew.h>

class Renderer
{
public:
	
	Renderer();

	template <typename Renderable>
	void render(Renderable &renderable) {
		
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
	
};

