#include "Rings.h"


Rings::Rings(float innerRadius, float outerRadius) : innerRadius(innerRadius), outerRadius(outerRadius)
{
	n_vertices = points * 2;
	n_triangulos = (points) * 2;
	vertices = new Position[points * 2];
	normal = new Position[points * 2];
	textCoord = new Position2D[points * 2];
	index = new int[n_triangulos * 3];

	for (int i = 0; i < points*2; i+=2) {
		vertices[i].x = sin(i*M_PI  / (float)points) * outerRadius;
		vertices[i].y = 0;
		vertices[i].z = cos(i*M_PI  / (float)points) * outerRadius;

		vertices[i + 1].x = sin(i*M_PI  / (float)points) * innerRadius;
		vertices[i + 1].y = 0;
		vertices[i + 1].z = cos(i*M_PI  / (float)points) * innerRadius;

		normal[i] = { 0, 1, 0 };
		normal[i + 1] = { 0, 1, 0 };

	}

	for (int i = 0; i < points * 2; i += 4) {
		textCoord[i] = { 1, 0 };
		textCoord[i + 1] = { 0, 1 };
		textCoord[i + 2] = { 0, 0 };
		textCoord[i + 3] = { 1, 1 };
	}

	int addedCircle = 0;
	for (int i = 0; i < n_triangulos*3; i+=6) {
		int vertice = addedCircle;
		index[i] = vertice;
		index[i + 1] = vertice + 1;
		index[i + 2] = vertice + 2;
		index[i + 3] = vertice + 1;
		index[i + 4] = vertice + 2;
		index[i + 5] = vertice + 3;

		addedCircle += 2;
	}

	index[n_triangulos * 3 - 1] = 1;
	index[n_triangulos * 3 - 2] = 0;
	index[n_triangulos * 3 - 4] = 0;

}


Rings::~Rings()
{
	delete[] vertices;
	delete[] normal;
	delete[] textCoord;
	delete[] index;
}
