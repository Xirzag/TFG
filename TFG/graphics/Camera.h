#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

class Camera
{
protected:
	glm::vec3 _position;
	glm::vec3 _front;
	float _rotation = 0;

public:
	Camera(glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 front = glm::vec3(0, 0, 1));

	glm::vec3 position();
	void position(glm::vec3 position);

	glm::vec3 front();
	void front(glm::vec3 front);
	void focus_point(glm::vec3 point);
	void rotate(float degress);
	float rotation();

	glm::mat4x4 virtual get_view_matrix();

};

class Camera3D : public Camera{

	float interocular_distance;

public:

	Camera3D(glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 front = glm::vec3(0, 0, 1), float interocular_distance = 1);

	enum Lens { left, right };

	glm::mat4x4 get_view_matrix(Lens lens);

	float lens_separation();
	void lens_separation(float separation);

private:

	float offset(Lens lens);

};