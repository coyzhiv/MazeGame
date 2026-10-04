#include <iostream>
#include <stdexcept>
#include <GLFW/glfw3.h>
#include "Maze/Maze.h"
#include "Maze/MazeGeometry.h"
#include "Renderer/VulkanContext.h"
#include "Renderer/Texture.h"

int main(int argc, char* argv[])
{
    if (!glfwInit())
    {
        return -1;
    }

    // GLFW не создаёт OpenGL context.
    // Рендеринг будет выполняться через Vulkan.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* window = glfwCreateWindow(
        1280,
        720,
        "Maze Game",
        nullptr,
        nullptr
    );

    uint32_t mazeSize = 31;

    if (argc > 1)
    {
        try
        {
            mazeSize =
                static_cast<uint32_t>(
                    std::stoul(argv[1])
                );
        }
        catch (...)
        {
            std::cerr
                << "Invalid maze size. "
                << "Use an odd number between 5 and 101."
                << '\n';

            return -1;
        }
    }

    if (mazeSize < 5 ||
        mazeSize > 101 ||
        mazeSize % 2 == 0)
    {
        std::cerr
            << "Invalid maze size. "
            << "Use an odd number between 5 and 101."
            << '\n';

        return -1;
    }

    Maze maze(
        mazeSize,
        mazeSize
    );

    maze.generate();

    std::vector<Vertex> mazeVertices =
        MazeGeometry::generate(maze);

    VulkanContext vulkanContext;

    vulkanContext.setMaze(&maze);
    vulkanContext.setVertices(mazeVertices);

    vulkanContext.initialize(window);

    TextureData texture = loadTexture(
        "assets/floor.png"
    );

    std::cout
        << "Texture loaded: "
        << texture.width
        << "x"
        << texture.height
        << " channels: "
        << texture.channels
        << '\n';

    freeTexture(texture);

    if (!window)
    {
        vulkanContext.cleanup();
        glfwTerminate();
        return -1;
    }

    float lastTime = static_cast<float>(glfwGetTime());

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        float currentTime =
            static_cast<float>(glfwGetTime());

        float deltaTime =
            currentTime - lastTime;

        lastTime = currentTime;

        vulkanContext.update(deltaTime);

        vulkanContext.drawFrame();
    }

    vulkanContext.cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}