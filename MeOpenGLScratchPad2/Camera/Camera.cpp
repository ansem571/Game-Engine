#include <Camera\Camera.h>
#include <glm\gtx\transform.hpp>
#include <windows.h>

const float Camera::MOVEMENT_SPEED = 0.1f;
const float Camera::ROTATIONAL_SPEED = 0.4f;

Camera::Camera() :
	viewDirection(0.0f, 0.0f, -1.0f),
	position(0.0270246491f, 3.73235941f, 5.02219105f),
	UP(0.0f, 1.0f, 0.0f)
{
}

void Camera::mouseUpdate(const glm::vec2& newMousePosition)
{
	glm::vec2 mouseDelta = newMousePosition - oldMousePosition;
	if(glm::length(mouseDelta) > 50.0f)
	{
		oldMousePosition = newMousePosition;
		return;
	}
	strafeDirection = glm::cross(viewDirection, UP);
	glm::mat4 rotator = glm::rotate(-mouseDelta.x * ROTATIONAL_SPEED, UP)*
						glm::rotate(-mouseDelta.y * ROTATIONAL_SPEED, strafeDirection);

	viewDirection = glm::mat3(rotator) * viewDirection;

	oldMousePosition = newMousePosition;
}

glm::mat4 Camera::getWorldToViewMatrix() const
{
	return glm::lookAt(position, position + viewDirection, UP);
}

void Camera::moveForward()
{
	position += MOVEMENT_SPEED * viewDirection;
}

void Camera::moveBackward()
{
	position += -MOVEMENT_SPEED * viewDirection;
}

void Camera::strafeLeft()
{
	position += -MOVEMENT_SPEED * strafeDirection;
}

void Camera::strafeRight()
{
	position += MOVEMENT_SPEED * strafeDirection;
}

void Camera::moveUp()
{
	position += MOVEMENT_SPEED * UP;
}

void Camera::moveDown()
{
	position += -MOVEMENT_SPEED * UP;
}

glm::vec3 Camera::getPosition()
{
	glm::vec3 pos;
	pos.x = position.x;
	pos.y = position.y;
	pos.z = position.z;
	return pos;
}