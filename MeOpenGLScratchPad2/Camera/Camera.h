#pragma once
#include <glm\glm.hpp>

class Camera
{
	glm::vec3 position;
	glm::vec3 viewDirection;
	const glm::vec3 UP;
	glm::vec2 oldMousePosition;
	static const float MOVEMENT_SPEED;
	static const float ROTATIONAL_SPEED;
	glm::vec3 strafeDirection;

public:
	Camera();
	glm::mat4 getWorldToViewMatrix() const;
	void mouseUpdate(const glm::vec2& newMousePosition);

	void moveForward();
	void moveBackward();
	void strafeLeft();
	void strafeRight();
	void moveUp();
	void moveDown();
	glm::vec3 getPosition();
};

