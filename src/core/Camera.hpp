#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera 
{
public:
  enum Direction 
  {
    FORWARD = 1,
    BACKWARD,
    LEFT,
    RIGHT
  };
public:
  Camera();
  glm::vec3 position() { return m_position; }
  glm::vec3 target() { return m_target; }
  const glm::vec3& position() const { return m_position; }
  const glm::vec3& target() const { return m_target; }
  const float sensivity() const { return m_sensivity; }
  const float speed() const { return m_base_speed; }
  bool freezed() const { return m_freezed; }
  glm::mat4 view_matrix() const;
  void freeze() { m_freezed = true; }
  void unfreeze() { m_freezed = false; }
  void set_position(const glm::vec3& position) { m_position = position; }
  void set_sensivity(float sensivity) { m_sensivity = sensivity; }
  void set_speed(float speed) { m_base_speed = speed; }
  void move(Direction direction);
  void scale_speed(float delta_time);
  void add_to_yaw_and_pitch(float x_offset, float y_offset);
  void update_camera_vectors();
  void look_at(const glm::vec3& position);
private:
  float m_pitch;  // how much we are looking up or down
  float m_yaw;   // magnitute of looking left or right
  float m_base_speed; // speed from ctor
  float m_actual_speed; // speed according to deltatime
  float m_sensivity; 
  bool m_freezed;
  glm::vec3 m_up;      // vector
  glm::vec3 m_target;  // vector
  glm::vec3 m_position; // point
};
