# Procedural Maze Explorer

A small first-person 3D procedural maze explorer created as a take-home assignment for the **C/C++ Graphics Developer** position.

The project is implemented in **C++17 using Vulkan**, with GLFW for window and input handling and GLM for mathematical operations.

## Overview

The application generates a new 3D maze every time it starts and allows the player to explore it from a first-person perspective.

The project was designed with the rendering and code architecture requirements of the assignment in mind rather than focusing on gameplay systems.

For the visual direction, I took some inspiration from **classic JRPG dungeon crawlers**. The goal was to keep the environment simple and readable while giving the maze the feeling of an old-school dungeon exploration game.

---

# Technical Choices

## Graphics API

I chose **Vulkan** for the rendering backend.

The assignment allowed the use of OpenGL, Vulkan, Direct3D12, Metal, or frameworks built on top of them. I chose Vulkan because it provides explicit control over the graphics pipeline, resources, synchronization, and command submission.

The renderer is structured around the main Vulkan components required to render the maze:

* Vulkan instance and physical device selection
* Logical device and graphics/present queues
* Swapchain
* Render pass
* Framebuffers
* Command pool and command buffers
* Graphics pipeline
* Vertex buffers
* Depth buffer
* Texture images and samplers
* Descriptor sets
* Uniform buffers
* Synchronization objects

## Maze Generation

The maze is generated procedurally using a **depth-first search / recursive backtracking style algorithm**.

The generation operates on a 2D grid. Cells are opened while traversing the grid in steps of two cells, removing the wall between the current cell and the selected neighboring cell.

This produces a connected maze while keeping the implementation relatively simple and deterministic in structure.

A new maze is generated every time the application starts.

## Maze Representation

The maze is represented internally as a 2D grid:

* Walkable cells represent corridors.
* Non-walkable cells represent walls.

The generated grid is then converted into 3D geometry before being passed to the renderer.

## Geometry

Maze walls are rendered as simple 3D box geometry.

The geometry contains:

* Position
* UV coordinates
* Texture index
* Surface normal

The floor, walls, ceiling, and exit marker use the same general vertex pipeline while selecting different textures or material behavior through the vertex data.

## Camera and Movement

The player uses a first-person camera.

Supported movement:

* `W` — forward
* `S` — backward
* `A` — strafe left
* `D` — strafe right
* Mouse — camera rotation

Movement uses acceleration and deceleration rather than instantly changing the player's velocity, resulting in smoother movement.

The camera uses a perspective projection and a view matrix generated with GLM.

## Collision Detection

Collision detection is implemented against the maze's non-walkable grid cells.

The player is treated as a small circle in the horizontal plane. Collision is resolved separately along the X and Z axes, preventing the player from moving through walls while still allowing smooth sliding along them.

The maze boundaries are formed by the generated outer wall cells, so the player cannot leave the maze area.

---

# Visual Features

## Textures

The assignment allowed either flat-colored walls or textured walls.

I chose to implement textured geometry.

The project uses separate textures for:

* Floor
* Walls
* Ceiling

Textures are loaded with `stb_image` and uploaded to Vulkan images using staging buffers.

Nearest-neighbor filtering is used to keep a deliberately simple, slightly retro visual appearance.

## Lighting

Basic directional lighting was implemented as an optional feature from the assignment.

Each vertex contains a surface normal, which is passed to the fragment shader.

The final lighting consists of:

* Ambient lighting
* Directional diffuse lighting

The lighting is intentionally kept simple so that the textures and geometry remain clearly visible.

## Floor and Ceiling

Both the floor and ceiling are rendered as separate geometry.

This was implemented as one of the optional features from the assignment and also helps reinforce the dungeon-like visual style.

## Exit

The maze contains a dedicated exit position.

The exit is represented by a simple animated cube. When the player reaches the exit, a victory state is triggered and the exit marker performs a visual effect.

---

# Debug and Development Features

Several small debugging features are available without adding a traditional in-game UI.

## Debug Map

Press:

```text
M
```

to toggle the debug map in the console.

The map represents the maze grid using:

```text
##  = wall
   = walkable space
P   = player
E   = exit
```

The player and exit are highlighted using console colors.

This is useful for verifying:

* Maze generation
* Start position
* Exit position
* Player movement
* Collision behavior
* Maze connectivity

## Adjustable Maze Size

The maze size can be provided as a command-line argument.

For example:

```text
MazeGame.exe 21
```

creates a 21 × 21 maze.

Other examples:

```text
MazeGame.exe 31
MazeGame.exe 51
MazeGame.exe 101
```

If no argument is provided, the default maze size is:

```text
31 × 31
```

The application accepts only **odd maze sizes from 5 to 101**.

Invalid examples:

```text
MazeGame.exe 20
MazeGame.exe 4
MazeGame.exe 102
```

These values are rejected because the maze generation algorithm relies on an odd-sized grid with boundary cells.

---

# Controls

| Key / Input          | Action                |
| -------------------- | --------------------- |
| `W`                  | Move forward          |
| `S`                  | Move backward         |
| `A`                  | Strafe left           |
| `D`                  | Strafe right          |
| Mouse                | Look around           |
| `M`                  | Toggle debug maze map |

---

# Running the Application

A ready-to-run Windows build is available in the **GitHub Releases** section of this repository.

Download the Release package and run:

```text
MazeGame.exe
```

The required assets and compiled shaders are included with the executable.

The application can also be launched with a custom maze size:

```text
MazeGame.exe 21
```

For development, the project can be configured and built using CMake.

## Dependencies

The project uses:

* C++17
* Vulkan SDK
* GLFW
* GLM
* stb_image
* CMake
* vcpkg

---

# Project Structure

```text
MazeGame/
├── assets/
│   ├── floor.png
│   ├── wall.png
│   └── ceiling.png
│
├── shaders/
│   ├── basic.vert
│   ├── basic.frag
│   └── compiled/
│       ├── basic.vert.spv
│       └── basic.frag.spv
│
├── src/
│   ├── Camera/
│   ├── Maze/
│   ├── Renderer/
│   └── main.cpp
│
├── CMakeLists.txt
├── CMakePresets.json
└── README.md
```

The `build` directory is intentionally not included in the source repository. A compiled Release build is provided separately through GitHub Releases.

---

# Assignment Feature Checklist

## Required Features

* [x] A 3D maze is generated every time the application starts.
* [x] The maze is based on a 2D grid.
* [x] Empty cells represent corridors that the player can walk through.
* [x] Filled cells represent walls.
* [x] Walls are rendered as simple 3D geometry.
* [x] Maze generation is procedural and produces a different layout on each run.
* [x] A procedural maze generation algorithm is used.
* [x] The player uses a first-person camera.
* [x] Forward movement is implemented.
* [x] Backward movement is implemented.
* [x] Strafing left and right is implemented.
* [x] Mouse look is implemented.
* [x] The player cannot walk through walls.
* [x] Basic collision detection is implemented.
* [x] The maze has clear boundaries.
* [x] The player cannot leave the maze area.

## Optional / Bonus Features

* [x] Basic directional lighting.
* [x] Textured walls.
* [x] Simple floor.
* [x] Simple ceiling.
* [ ] Instanced rendering / batching for maze geometry.
* [x] Minimap / debug visualization of the maze grid.
* [x] Adjustable maze size.
* [x] Visual effect when reaching the maze exit.

## Technology

* [x] Vulkan rendering backend.
* [x] C++17.
* [x] CMake-based project.
* [x] GLFW window and input handling.
* [x] GLM mathematics library.

---

# Notes

The project intentionally keeps the gameplay layer small. The main focus was placed on:

* Rendering architecture
* Vulkan resource management
* Procedural geometry generation
* Camera and movement
* Collision handling
* Texture management
* GPU synchronization
* Clean separation between the maze, camera, and renderer

The debug functionality is kept outside the rendered scene so that the application remains consistent with the assignment requirement of having no UI.
