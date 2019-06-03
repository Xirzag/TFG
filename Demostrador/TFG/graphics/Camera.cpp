#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 point) : _position(position), _front(point)
{
	recalculate_vectors();
}

glm::vec3 Camera::position()
{
	return _position;
}

void Camera::position(glm::vec3 position)
{
	_position = position;
}

void Camera::recalculate_vectors()
{
	_right = glm::normalize(glm::cross(_front, glm::vec3(0, 1, 0)));
	_right = glm::vec3(glm::rotate(glm::mat4(1), _rotation, _front) * glm::vec4(_right, 0));
	_up = glm::normalize(glm::cross(_right, _front));
}

glm::vec3 Camera::front()
{
	return _front;
}

void Camera::front(glm::vec3 front)
{
	_front = front;
	recalculate_vectors();
}

void Camera::focus_point(glm::vec3 point)
{
	_front = glm::normalize(point - _position);
	recalculate_vectors();
}

void Camera::rotate(float radians)
{
	_rotation = radians;
	recalculate_vectors();
}

void Camera::screen_dimensions(glm::ivec2 dimensions)
{
	this->dimensions = dimensions;
}

void Camera::field_of_view(float fov)
{
	this->fov = fov;
}

float Camera::field_of_view()
{
	return fov;
}

float Camera::rotation()
{
	return _rotation;
}

float Camera::aspect_ratio()
{
	return float(dimensions.x) / float(dimensions.y);
}

void Camera::near_frustum(float distance)
{
	_near = distance;
}

void Camera::far_frustum(float distance)
{
	_far = distance;
}

float Camera::near_frustum()
{
	return _near;
}

float Camera::far_frustum()
{
	return _far;
}

glm::mat4x4 Camera::get_view_matrix()
{
	return glm::lookAt(_position, _position + _front, _up);
}

glm::mat4x4 Camera::get_projection_matrix()
{
	return glm::perspective(fov, aspect_ratio(), _near, _far);
}


Camera3D::Camera3D(glm::vec3 position, glm::vec3 front, float interocular_distance) : Camera(position, front), interocular_distance(interocular_distance)
{
}

Camera3D::Lens Camera3D::lens(bool lens)
{
	return lens? right : left;
}

glm::mat4x4 Camera3D::get_view_matrix(Lens lens)
{	
	recalculate_vectors();
	const glm::vec3 lens_position = _position + offset(lens) * _right;
	return glm::lookAt(lens_position, lens_position + _front, _up);
}

glm::mat4x4 Camera3D::get_projection_matrix(Lens lens)
{

	const float wd2 = _near * tan(fov / 2.f);
	const float ndfl = _near / center;

	const float coef = offset(lens);
	const float ratio = aspect_ratio();

	const float left = -ratio * wd2 + coef * ndfl;
	const float right = ratio * wd2 + coef * ndfl;
	const float top = wd2;
	const float bottom = -wd2;

	return glm::frustum(left, right, bottom, top, _near, _far);
}

float Camera3D::lens_separation()
{
	return interocular_distance;
}

void Camera3D::lens_separation(float separation)
{
	interocular_distance = separation;
}

void Camera3D::set_center(float distance)
{
	center = distance;
}

float Camera3D::offset(Lens lens)
{
	return (lens == left? -interocular_distance : interocular_distance) / 2.f;
}

CameraToeIn::CameraToeIn(glm::vec3 position, glm::vec3 front, float interocular_distance) 
	: Camera3D(position, front, interocular_distance) { }

glm::mat4x4 CameraToeIn::get_view_matrix(Lens lens)
{
	const glm::vec3 lens_position = _position + offset(lens) * _right;
	return glm::lookAt(lens_position, _position + _front * center + _front, _up);
}

glm::mat4x4 CameraToeIn::get_projection_matrix(Lens lens)
{
	return Camera::get_projection_matrix();
}

CameraOffsetFrustums::CameraOffsetFrustums(glm::vec3 position, glm::vec3 front, float interocular_distance)
	: Camera3D(position, front, interocular_distance) { }

glm::mat4x4 CameraOffsetFrustums::get_view_matrix(Lens lens)
{
	return Camera3D::get_view_matrix(lens);
}

glm::mat4x4 CameraOffsetFrustums::get_projection_matrix(Lens lens)
{
	return Camera::get_projection_matrix();
}
