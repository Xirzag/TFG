#ifndef ESFERA_H
#define ESFERA_H

/*
Una clase que construye los datos: vertices, normales y 
coordenadas de textura de una Sphere
*/

class Sphere{
public:
	Sphere(float, int, int);
	~Sphere();
	int N, M;
	int n_triangles;
	int n_vertices;
	float *vertices;
	float *normal;
	float *textCoord;
	int *index;

protected:
	double *theta, *phi;
	double R;
	int get_index(int, int);
};

#endif