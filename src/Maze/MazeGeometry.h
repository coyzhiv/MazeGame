#pragma once

#include "Maze.h"

#include "../Renderer/Vertex.h"

#include <vector>

class MazeGeometry
{
public:
    static std::vector<Vertex> generate(
        const Maze& maze
    );
};