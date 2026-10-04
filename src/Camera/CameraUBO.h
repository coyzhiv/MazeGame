#pragma once

#include <glm/glm.hpp>

struct CameraUBO
{
    glm::mat4 view;
    glm::mat4 projection;
    float time;
    float victoryTime;
};