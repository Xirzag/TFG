#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

class Camera
{
protected:
	glm::vec3 _position;
	glm::vec3 _front;
	glm::vec3 _right;
	glm::vec3 _up;

	float _rotation = 0;
	glm::ivec2 dimensions;
	float fov;
	float _near = 0.1f;
	float _far = 100.f;

	void recalculate_vectors();

public:
	Camera(glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 front = glm::vec3(0, 0, 1));

	glm::vec3 position();
	void position(glm::vec3 position);

	//! Returns the camera current direction
	glm::vec3 front();
	//! Set camera direction
	void front(glm::vec3 front);
	//! Change the camera direction to focus a point
	void focus_point(glm::vec3 point);
	//! roll the camera using degress
	void rotate(float degress);
	//! set screen dimensions in pixels
	void screen_dimensions(glm::ivec2 dimensions);
	//! field of view in degress
	void field_of_view(float fov);
	//! field of view in degress
	float field_of_view();
	//! camera roll using degress
	float rotation();
	float aspect_ratio();
	void near_frustum(float distance);
	void far_frustum(float distance);
	float near_frustum();
	float far_frustum();

	glm::mat4x4 virtual get_view_matrix();
	glm::mat4x4 virtual get_projection_matrix();

};

//! Implements a 3D camera using the OffAxis method
class Camera3D : public Camera{
public:

	Camera3D(glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 front = glm::vec3(0, 0, 1), float interocular_distance = 1);

	//! Enum used to specify the lens of the stereo camera
	enum Lens { left, right };
	Lens lens(bool isRight);

	//! Returns the view matrix for a specific lens
	glm::mat4x4 virtual get_view_matrix(Lens lens);
	//! Returns the projection matrix for a specific lens
	glm::mat4x4 virtual get_projection_matrix(Lens lens);

	float lens_separation();
	void lens_separation(float separation);

	//! Set the center of the 3D camera.
	//! The objects behind the distance will seem to be beyond the screen
	void set_center(float distance);

protected:

	float interocular_distance;
	float center;

	float offset(Lens lens);

};


class CameraToeIn : public Camera3D {

public:

	CameraToeIn(glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 front = glm::vec3(0, 0, 1), float interocular_distance = 1);

	glm::mat4x4 get_view_matrix(Lens lens);
	glm::mat4x4 get_projection_matrix(Lens lens);

};

class CameraOffsetFrustums : public Camera3D {

public:

	CameraOffsetFrustums(glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 front = glm::vec3(0, 0, 1), float interocular_distance = 1);

	glm::mat4x4 get_view_matrix(Lens lens);
	glm::mat4x4 get_projection_matrix(Lens lens);

	//! Don't do anything
	void set_center(float distance) {};

};