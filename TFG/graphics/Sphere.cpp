#include <cstdio>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <cmath>

#include "Sphere.h"

Sphere::Sphere(float r, int n, int m){

	N = n;
	M = m+1;
	R = (double)r;

	theta = (double*)malloc(N*sizeof(double));
	phi = (double*)malloc(M*sizeof(double));
	for (int i = 0; i < N; i++) theta[i] = (double)i*M_PI / (double)(N - 1);
	// realmente M-1 meridianos diferentes ya que el primero y último coinciden
	for (int i = 0; i < M; i++) phi[i] = (double)i*2.0*M_PI / (double)(M - 1); 

	n_vertices = 2 + (N - 2)*M;
	vertices = (float *)malloc(3 * n_vertices*sizeof(float));
	normal = (float *)malloc(3 * n_vertices*sizeof(float));
	textCoord = (float *)malloc(2 * n_vertices*sizeof(float));
	int index_vertice = 0;
	// punto polar norte
	vertices[3*index_vertice + 0] = 0.0f; // componente x
	vertices[3*index_vertice + 1] = (float)R; // componente y
	vertices[3*index_vertice + 2] = 0.0f; // componente z

	normal[3*index_vertice + 0] = 0.0f; // componente x
	normal[3*index_vertice + 1] = 0.0f; // componente y
	normal[3*index_vertice + 2] = 1.0f; // componente z

	textCoord[2*index_vertice + 0] = 0.5f; // componente u
	textCoord[2*index_vertice + 1] = 1.0f; // componente v
	index_vertice++;

	for (int i = 1; i < N - 1; i++){
		for (int j = 0; j < M; j++){

			double sint = sin(theta[i]);
			double x = sint*cos(phi[j]);
			double y = sint*sin(phi[j]);
			double z = cos(theta[i]);

			vertices[3*index_vertice + 0] = (float)(x*R); // componente x
			vertices[3*index_vertice + 1] = (float)(z*R); // componente y
			vertices[3*index_vertice + 2] = (float)(y*R); // componente z
			normal[3*index_vertice + 0] = (float)x; // componente x
			normal[3*index_vertice + 1] = (float)y; // componente y
			normal[3*index_vertice + 2] = (float)z; // componente z
			textCoord[2*index_vertice + 0] = (float)(M-j)/(float)(M-1);
			textCoord[2*index_vertice + 1] = (float)(N-1-i)/(float)(N-1);
			index_vertice++;
		}
	}

	// punto polar sur
	vertices[3*index_vertice + 0] = 0.0f; // componente x
	vertices[3*index_vertice + 1] = -(float)R; // componente y
	vertices[3*index_vertice + 2] = 0.0f; // componente z

	normal[3*index_vertice + 0] = 0.0f; // componente x
	normal[3*index_vertice + 1] = 0.0f; // componente y
	normal[3*index_vertice + 2] = -1.0f; // componente z

	textCoord[2*index_vertice + 0] = 0.5f; // componente u
	textCoord[2*index_vertice + 1] = 0.0f; // componente v
	index_vertice++;

	//printf("%d %d\n", n_vertices, index_vertice);

	// para codificar los índices de los triangulos
	// polo norte
	n_triangles = 2*(M-1)+2*(M-1)*(N-2);
	index = (int *)malloc(3* n_triangles *sizeof(int));
	int index_triangulo = 0;
	n_triangles = 0;

	for (int j = 0; j < M-1; j++){
		index[3*index_triangulo + 0] = 0;
		index[3*index_triangulo + 1] = j+1;
		index[3*index_triangulo + 2] = j+2;
		index_triangulo ++;
		n_triangles++;
	}

	// bandas de paralelos
	for (int i = 1; i < N - 1; i++){
		for (int j = 0; j < M - 1; j++){
			index[3 * index_triangulo + 0] = get_index(i,j);
			index[3 * index_triangulo + 1] = get_index(i+1,j);
			index[3 * index_triangulo + 2] = get_index(i+1,j+1);
			index_triangulo++;
			n_triangles++;

			index[3 * index_triangulo + 0] = get_index(i, j);
			index[3 * index_triangulo + 1] = get_index(i + 1, j + 1);
			index[3 * index_triangulo + 2] = get_index(i, j + 1);
			index_triangulo++;
			n_triangles++;
		}
	}

	
}

Sphere::~Sphere(){

	free(theta);
	free(phi);

	free(vertices);
	free(normal);
	free(textCoord);
	free(index);
}

int Sphere::get_index(int a, int b){

	if (a == 0) return 0;
	if (a == N - 1) return (N - 2)*M + 1;
	return (a - 1)*M + b + 1;
}