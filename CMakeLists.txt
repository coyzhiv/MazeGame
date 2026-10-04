cmake_minimum_required(VERSION 3.20)

project(MazeGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VULKAN_SDK "C:/VulkanSDK/1.4.363.0")

find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(
    MazeGame
    src/main.cpp

    src/Renderer/VulkanContext.cpp
    src/Renderer/VulkanContext.h
    src/Renderer/Vertex.h

    src/Renderer/Texture.cpp
    src/Renderer/Texture.h

    src/Camera/Camera.cpp
    src/Camera/Camera.h

    src/Maze/Maze.cpp
    src/Maze/Maze.h
    src/Maze/MazeGeometry.cpp
    src/Maze/MazeGeometry.h
)

target_include_directories(
    MazeGame
    PRIVATE
    "${VULKAN_SDK}/Include"
)

target_link_libraries(
    MazeGame
    PRIVATE
    Vulkan::Vulkan
    glfw
    glm::glm
)

add_custom_command(TARGET MazeGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:MazeGame>/assets"

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/shaders/compiled"
        "$<TARGET_FILE_DIR:MazeGame>/shaders/compiled"
)