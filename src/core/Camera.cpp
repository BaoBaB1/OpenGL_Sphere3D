#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.hpp"
#include "macro.hpp"

Camera::Camera() : Camera(glm::vec3(), 5.f, 0.2f) {

}

Camera::Camera(const glm::vec3& position, float speed, float sensivity) {
	m_position = position;
	m_actual_speed = m_base_speed = speed;
	m_sensivity = sensivity;
	m_yaw = -90.f;
	m_pitch = 0.f;
	m_up = glm::vec3(0.f, 1.f, 0.f);
	m_target = glm::vec3(0.f);
}

void Camera::move(Direction direction) {
	switch (direction)
	{
	case Direction::FORWARD: 
		m_position += m_actual_speed * m_target;
		break;
	case Direction::BACKWARD:
		m_position -= m_actual_speed * m_target;
		break;
	case Direction::LEFT:
		m_position -= glm::normalize(glm::cross(m_target, m_up)) * m_actual_speed;
		break;
	case Direction::RIGHT:
		m_position += glm::normalize(glm::cross(m_target, m_up)) * m_actual_speed;
		break;
	default:
		break;
	}
}

void Camera::scale_speed(float delta_time) {
	m_actual_speed = m_base_speed * delta_time;
}

glm::mat4 Camera::view_matrix() {
	return glm::lookAt(m_position, m_position + m_target, m_up);
}

void Camera::add_to_yaw_and_pitch(float x_offset, float y_offset) {
	m_yaw += x_offset * m_sensivity;
	m_pitch += y_offset * m_sensivity;
	update_camera_vectors();
}

void Camera::update_camera_vectors() {
	glm::vec3 dir;
	if (m_pitch > 89.0f)
		m_pitch = 89.0f;
	if (m_pitch < -89.0f)
		m_pitch = -89.0f;
	dir.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	dir.y = -sin(glm::radians(m_pitch)); // -sin ?
	dir.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	// !!!
	// camera may jump once after first mouse event. [check dir vector and m_target vector which is set in Camera::look_at() before.] 
	// coordinates may be represented as permutation of new dir vector and previously calculated m_target in Camera::look_at()
	// may jump or might ... need to investigate more ...
	// possible bug in pitch and yaw calculation with signs in atan in Camera::look_at() 
	m_target = glm::normalize(dir);
}

void Camera::look_at(const glm::vec3& target) {
	assert(target != m_position);
	glm::vec3 camera_dir = glm::normalize(target - m_position);
	m_yaw = std::atan2(camera_dir.z, camera_dir.x); // -x is possible also ?
	m_pitch = std::asin(-camera_dir.y);
	m_yaw = glm::degrees(m_yaw);
	m_pitch = glm::degrees(m_pitch);
	m_target = camera_dir;
}