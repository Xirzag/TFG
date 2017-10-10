#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 point) : _position(position), _front(point)
{
}

glm::vec3 Camera::position()
{
	return _position;
}

void Camera::position(glm::vec3 position)
{
	_position = position;
}

glm::vec3 Camera::front()
{
	return _front;
}

void Camera::front(glm::vec3 front)
{
	_front = front;
}

void Camera::focus_point(glm::vec3 point)
{
	_front = glm::normalize(point - _position);
}

void Camera::rotate(float radians)
{
	_rotation = radians;
}

float Camera::rotation()
{
	return _rotation;
}

glm::mat4x4 Camera::get_view_matrix()
{
	return glm::lookAt(_position, _position + _front, glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1.0f), _rotation, _front);
}


Camera3D::Camera3D(glm::vec3 position, glm::vec3 front, float interocular_distance) : Camera(position, front), interocular_distance(interocular_distance)
{
}

glm::mat4x4 Camera3D::get_view_matrix(Lens lens)
{
	glm::vec3 lens_position = _position + glm::vec3(offset(lens), 0, 0);
	return glm::lookAt(lens_position, lens_position + _front, glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1.0f), _rotation, _front);
}

float Camera3D::lens_separation()
{
	return interocular_distance;
}

void Camera3D::lens_separation(float separation)
{
	interocular_distance = separation;
}

float Camera3D::offset(Lens lens)
{
	return (lens == left? -interocular_distance : interocular_distance) / 2.f;
}
