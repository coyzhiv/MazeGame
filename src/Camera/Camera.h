#pragma once

#include <glm/glm.hpp>

class Camera
{
public:

    Camera();

    void setMovementInput(
        float forwardInput,
        float rightInput
    );

    void setPosition(
        const glm::vec3& newPosition
    );

    void update(float deltaTime);

    void processMouseMovement(
        float xOffset,
        float yOffset
    );

    glm::mat4 getViewMatrix() const;

    glm::vec3 getPosition() const;

private:
    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 up;

    glm::vec3 velocity;

    glm::vec2 movementInput;

    float yaw;
    float pitch;
};