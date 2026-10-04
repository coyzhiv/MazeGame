#include "Maze.h"

#include <algorithm>
#include <array>
#include <random>
#include <stdexcept>

Maze::Maze(
    uint32_t width,
    uint32_t height)
    : width(width),
      height(height),
      startX(1),
      startY(1),
      exitX(width - 2),
      exitY(height - 2),
      cells(width * height, 0)
{
}

void Maze::generate()
{
    // Start with everything as a wall.
    std::fill(
        cells.begin(),
        cells.end(),
        0
    );

    // Maze generation works best with odd dimensions.
    if (width < 3 || height < 3 ||
        width % 2 == 0 ||
        height % 2 == 0)
    {
        throw std::runtime_error(
            "Maze dimensions must be odd and at least 3"
        );
    }

    std::random_device randomDevice;
    std::mt19937 randomEngine(randomDevice());

    // Stack for recursive-backtracking algorithm.
    std::vector<std::pair<uint32_t, uint32_t>> stack;

    // Start at (1, 1).
    uint32_t startX = 1;
    uint32_t startY = 1;

    cells[startY * width + startX] = 1;

    stack.push_back({ startX, startY });

    const std::array<std::pair<int, int>, 4> directions =
    {{
        { 2,  0},
        {-2,  0},
        { 0,  2},
        { 0, -2}
    }};

    while (!stack.empty())
    {
        auto [currentX, currentY] =
            stack.back();

        std::array<int, 4> directionIndices =
        {{
            0, 1, 2, 3
        }};

        std::shuffle(
            directionIndices.begin(),
            directionIndices.end(),
            randomEngine
        );

        bool foundUnvisitedCell = false;

        for (int directionIndex : directionIndices)
        {
            int nextX =
                static_cast<int>(currentX) +
                directions[directionIndex].first;

            int nextY =
                static_cast<int>(currentY) +
                directions[directionIndex].second;

            if (nextX <= 0 ||
                nextX >= static_cast<int>(width) - 1 ||
                nextY <= 0 ||
                nextY >= static_cast<int>(height) - 1)
            {
                continue;
            }

            if (cells[nextY * width + nextX] != 0)
            {
                continue;
            }

            // Remove the wall between the cells.
            int wallX =
                static_cast<int>(currentX) +
                directions[directionIndex].first / 2;

            int wallY =
                static_cast<int>(currentY) +
                directions[directionIndex].second / 2;

            cells[wallY * width + wallX] = 1;

            // Make the new cell walkable.
            cells[nextY * width + nextX] = 1;

            stack.push_back({
                static_cast<uint32_t>(nextX),
                static_cast<uint32_t>(nextY)
            });

            foundUnvisitedCell = true;

            break;
        }

        if (!foundUnvisitedCell)
        {
            stack.pop_back();
        }
    }
}

bool Maze::isWalkable(
    uint32_t x,
    uint32_t y) const
{
    if (x >= width || y >= height)
    {
        return false;
    }

    return cells[y * width + x] != 0;
}

uint32_t Maze::getWidth() const
{
    return width;
}

uint32_t Maze::getHeight() const
{
    return height;
}

uint32_t Maze::getStartX() const
{
    return startX;
}

uint32_t Maze::getStartY() const
{
    return startY;
}

uint32_t Maze::getExitX() const
{
    return exitX;
}

uint32_t Maze::getExitY() const
{
    return exitY;
}