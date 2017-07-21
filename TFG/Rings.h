#pragma once
#define _USE_MATH_DEFINES
#include <cmath>

struct Position{
	float x, y, z;
};

struct Position2D{
	float x, y;
};

class Rings
{
public:
	int points = 100;
	Rings(float, float);
	~Rings();
	float innerRadius, outerRadius;
	int n_triangulos;
	int n_vertices;
	Position *vertices;
	Position *normal;
	Position2D *textCoord;
	int *index;

protected:
	double *theta, *phi;
	double R;
};

