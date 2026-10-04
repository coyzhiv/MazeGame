#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
    : position(-14.0f, 0.6f, -14.0f),
      forward(1.0f, 0.0f, 0.0f),
      up(0.0f, 1.0f, 0.0f),
      velocity(0.0f, 0.0f, 0.0f),
      movementInput(0.0f, 0.0f),
      yaw(0.0f),
      pitch(0.0f)
{
}



void Camera::update(float deltaTime)
{
    const float acceleration = 30.0f;
    const float deceleration = 30.0f;
    const float maxSpeed = 3.0f;

    glm::vec3 right =
        glm::normalize(
            glm::cross(forward, up)
        );

    glm::vec3 desiredDirection =
        forward * movementInput.x +
        right * movementInput.y;

    if (glm::length(desiredDirection) > 0.001f)
    {
        desiredDirection =
            glm::normalize(desiredDirection);
    }

    glm::vec3 targetVelocity =
        desiredDirection * maxSpeed;

    glm::vec3 velocityDifference =
        targetVelocity - velocity;

    float accelerationAmount =
        acceleration * deltaTime;

    if (glm::length(velocityDifference) <=
        accelerationAmount)
    {
        velocity = targetVelocity;
    }
    else
    {
        velocity +=
            glm::normalize(velocityDifference) *
            accelerationAmount;
    }

    if (glm::length(movementInput) < 0.001f)
    {
        float speed =
            glm::length(velocity);

        float decelerationAmount =
            deceleration * deltaTime;

        if (speed <= decelerationAmount)
        {
            velocity = glm::vec3(0.0f);
        }
        else
        {
            velocity -=
                glm::normalize(velocity) *
                decelerationAmount;
        }
    }

    position += velocity * deltaTime;
}

void Camera::setMovementInput(
    float forwardInput,
    float rightInput)
{
    movementInput =
        glm::vec2(
            forwardInput,
            rightInput
        );

    float inputLength =
        glm::length(movementInput);

    if (inputLength > 1.0f)
    {
        movementInput /= inputLength;
    }
}

void Camera::processMouseMovement(
    float xOffset,
    float yOffset)
{
    const float sensitivity = 0.1f;

    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f)
    {
        pitch = 89.0f;
    }

    if (pitch < -89.0f)
    {
        pitch = -89.0f;
    }

    glm::vec3 direction;

    direction.x =
        cos(glm::radians(yaw)) *
        cos(glm::radians(pitch));

    direction.y =
        sin(glm::radians(pitch));

    direction.z =
        sin(glm::radians(yaw)) *
        cos(glm::radians(pitch));

    forward =
        glm::normalize(direction);
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(
        position,
        position + forward,
        up
    );
}

glm::vec3 Camera::getPosition() const
{
    return position;
}

void Camera::setPosition(
    const glm::vec3& newPosition)
{
    position = newPosition;
}

