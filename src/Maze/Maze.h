#pragma once

#include <vector>
#include <cstdint>

class Maze
{
public:
    Maze(uint32_t width, uint32_t height);

    void generate();

    bool isWalkable(uint32_t x, uint32_t y) const;

    uint32_t getWidth() const;
    uint32_t getHeight() const;
    uint32_t getStartX() const;
    uint32_t getStartY() const;

    uint32_t getExitX() const;
    uint32_t getExitY() const;

private:
    uint32_t width;
    uint32_t height;

    std::vector<uint8_t> cells;
    uint32_t startX;
    uint32_t startY;

    uint32_t exitX;
    uint32_t exitY;
};