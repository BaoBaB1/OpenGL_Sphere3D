#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
	enum Direction {
		FORWARD = 1,
		BACKWARD,
		LEFT,
		RIGHT,
		UNKNOWN
	};
public:
	Camera(const glm::vec3& initial_position, float speed, float sensivity);
	void move(Direction direction);
	void scale_speed(float delta_time);
	glm::vec3& position() { return m_position; }
	const glm::vec3& position() const { return m_position; }
	glm::mat4 view_matrix();
	void add_to_yaw_and_pitch(float x_offset, float y_offset);
	void update_camera_vectors();
private:
	float m_pitch;  // how much we are looking up or down
	float m_yaw;   // magnitute of looking left or right
	float m_base_speed; // speed from ctor
	float m_actual_speed; // speed according to deltatime
	float m_sensivity; 
	glm::vec3 m_up;
	glm::vec3 m_target;
	glm::vec3 m_position;
};
