#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.hpp"

Camera::Camera()
{
  m_position = glm::vec3(0.f);
  m_actual_speed = m_base_speed = 5.f;
  m_sensivity = 0.2f;
  m_yaw = -90.f;
  m_pitch = 0.f;
  m_up = glm::vec3(0.f, 1.f, 0.f);
  m_target = glm::vec3(0.f);
  m_freezed = false;
}

void Camera::move(Direction direction) 
{
  if (freezed())
    return;
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

void Camera::scale_speed(float delta_time) 
{
  m_actual_speed = m_base_speed * delta_time;
}

glm::mat4 Camera::view_matrix() const
{
  return glm::lookAt(m_position, m_position + m_target, m_up);
}

void Camera::add_to_yaw_and_pitch(float x_offset, float y_offset) 
{
  if (freezed())
    return;
  m_yaw += x_offset * m_sensivity;
  m_pitch += y_offset * m_sensivity;
  update_camera_vectors();
}

void Camera::update_camera_vectors() 
{
  glm::vec3 dir;
  if (m_pitch > 89.0f)
    m_pitch = 89.0f;
  if (m_pitch < -89.0f)
    m_pitch = -89.0f;
  dir.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
  dir.y = -sin(glm::radians(m_pitch)); // -sin ?
  dir.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
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
