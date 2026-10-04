#include "MazeGeometry.h"

std::vector<Vertex> MazeGeometry::generate(
    const Maze& maze)
{
    const uint32_t exitX = maze.getExitX();
    const uint32_t exitY = maze.getExitY();
    std::vector<Vertex> vertices;

    const float wallHeight = 1.0f;

    for (uint32_t y = 0;
         y < maze.getHeight();
         y++)
    {
        for (uint32_t x = 0;
             x < maze.getWidth();
             x++)
        {
            if (maze.isWalkable(x, y))
            {
                float mazeCenterX =
                    static_cast<float>(maze.getWidth() - 1) / 2.0f;

                float mazeCenterZ =
                    static_cast<float>(maze.getHeight() - 1) / 2.0f;

                float centerX =
                    static_cast<float>(x) -
                    mazeCenterX;

                float centerZ =
                    static_cast<float>(y) -
                    mazeCenterZ;

                float minX = centerX - 0.5f;
                float maxX = centerX + 0.5f;

                float minZ = centerZ - 0.5f;
                float maxZ = centerZ + 0.5f;

                // Floor
                vertices.push_back({
                    { minX, 0.0f, minZ },
                    { 0.0f, 0.0f },
                    0.0f,
                    { 0.0f, 1.0f, 0.0f }
                });

                vertices.push_back({
                    { maxX, 0.0f, minZ },
                    { 1.0f, 0.0f },
                    0.0f,
                    { 0.0f, 1.0f, 0.0f }   
                });

                vertices.push_back({
                    { maxX, 0.0f, maxZ },
                    { 1.0f, 1.0f },
                    0.0f,
                    { 0.0f, 1.0f, 0.0f }
                });

                vertices.push_back({
                    { minX, 0.0f, minZ },
                    { 0.0f, 0.0f },
                    0.0f,
                    { 0.0f, 1.0f, 0.0f }
                });

                vertices.push_back({
                    { maxX, 0.0f, maxZ },
                    { 1.0f, 1.0f },
                    0.0f,
                    { 0.0f, 1.0f, 0.0f }
                });

                vertices.push_back({
                    { minX, 0.0f, maxZ },
                    { 0.0f, 1.0f },
                    0.0f,
                    { 0.0f, 1.0f, 0.0f }
                });

                // Ceiling
                vertices.push_back({
                    { minX, wallHeight, minZ },
                    { 0.0f, 0.0f },
                    2.0f,
                    { 0.0f, -1.0f, 0.0f }
                });

                vertices.push_back({
                    { minX, wallHeight, maxZ },
                    { 0.0f, 1.0f },
                    2.0f,
                    { 0.0f, -1.0f, 0.0f }
                });

                vertices.push_back({
                    { maxX, wallHeight, maxZ },
                    { 1.0f, 1.0f },
                    2.0f,
                    { 0.0f, -1.0f, 0.0f }
                });

                vertices.push_back({
                    { minX, wallHeight, minZ },
                    { 0.0f, 0.0f },
                    2.0f,
                    { 0.0f, -1.0f, 0.0f }
                });

                vertices.push_back({
                    { maxX, wallHeight, maxZ },
                    { 1.0f, 1.0f },
                    2.0f,
                    { 0.0f, -1.0f, 0.0f }
                });

                vertices.push_back({
                    { maxX, wallHeight, minZ },
                    { 1.0f, 0.0f },
                    2.0f,
                    { 0.0f, -1.0f, 0.0f }
                });

                
                continue;
            }

            float mazeCenterX =
                static_cast<float>(maze.getWidth() - 1) / 2.0f;

            float mazeCenterZ =
                static_cast<float>(maze.getHeight() - 1) / 2.0f;

            float centerX =
                static_cast<float>(x) -
                mazeCenterX;

            float centerZ =
                static_cast<float>(y) -
                mazeCenterZ;

            float minX =
                centerX - 0.5f;

            float maxX =
                centerX + 0.5f;

            float minZ =
                centerZ - 0.5f;

            float maxZ =
                centerZ + 0.5f;

            float minY = 0.0f;
            float maxY = 1.0f;

            // Front face
            vertices.push_back({
                { minX, minY, minZ },
                { 0.0f, 0.0f },
                1.0f,
                { 0.0f, 0.0f, 1.0f }
            });

            vertices.push_back({
                { maxX, minY, minZ },
                { 1.0f, 0.0f },
                1.0f,
                { 0.0f, 0.0f, 1.0f }
            });

            vertices.push_back({
                { maxX, maxY, minZ },
                { 1.0f, 1.0f },
                1.0f,
                { 0.0f, 0.0f, 1.0f }
            });

            vertices.push_back({
                { minX, minY, minZ },
                { 0.0f, 0.0f },
                1.0f,
                { 0.0f, 0.0f, 1.0f }
            });

            vertices.push_back({
                { maxX, maxY, minZ },
                { 1.0f, 1.0f },
                1.0f,
                { 0.0f, 0.0f, 1.0f }
            });

            vertices.push_back({
                { minX, maxY, minZ },
                { 0.0f, 1.0f },
                1.0f,
                { 0.0f, 0.0f, 1.0f }
            });

            // Back face
            vertices.push_back({
                { maxX, minY, maxZ },
                { 0.0f, 0.0f },
                1.0f,
                { 0.0f, 0.0f, -1.0f }
            });

            vertices.push_back({
                { minX, minY, maxZ },
                { 1.0f, 0.0f },
                1.0f,
                { 0.0f, 0.0f, -1.0f }
            });

            vertices.push_back({
                { minX, maxY, maxZ },
                { 1.0f, 1.0f },
                1.0f,
                { 0.0f, 0.0f, -1.0f }
            });

            vertices.push_back({
                { maxX, minY, maxZ },
                { 0.0f, 0.0f },
                1.0f,
                { 0.0f, 0.0f, -1.0f }
            });

            vertices.push_back({
                { minX, maxY, maxZ },
                { 1.0f, 1.0f },
                1.0f,
                { 0.0f, 0.0f, -1.0f }
            });

            vertices.push_back({
                { maxX, maxY, maxZ },
                { 0.0f, 1.0f },
                1.0f,
                { 0.0f, 0.0f, -1.0f }
            });

            // Left face
            vertices.push_back({
                { minX, minY, maxZ },
                { 0.0f, 0.0f },
                1.0f,
                { -1.0f, 0.0f, 0.0f }
            });

            vertices.push_back({
                { minX, minY, minZ },
                { 1.0f, 0.0f },
                1.0f,
                { -1.0f, 0.0f, 0.0f }
            });

            vertices.push_back({
                { minX, maxY, minZ },
                { 1.0f, 1.0f },
                1.0f,
                { -1.0f, 0.0f, 0.0f }
            });

            vertices.push_back({
                { minX, minY, maxZ },
                { 0.0f, 0.0f },
                1.0f,
                { -1.0f, 0.0f, 0.0f }
            });

            vertices.push_back({
                { minX, maxY, minZ },
                { 1.0f, 1.0f },
                1.0f,
                { -1.0f, 0.0f, 0.0f }
            });

            vertices.push_back({
                { minX, maxY, maxZ },
                { 0.0f, 1.0f },
                1.0f,
                { -1.0f, 0.0f, 0.0f }
            });

            // Right face
            vertices.push_back({
                { maxX, minY, minZ },
                { 0.0f, 0.0f },
                1.0f,
                { 1.0f, 0.0f, 0.0f }
            });

            vertices.push_back({
                { maxX, minY, maxZ },
                { 1.0f, 0.0f },
                1.0f,
                { 1.0f, 0.0f, 0.0f }
            });

            vertices.push_back({
                { maxX, maxY, maxZ },
                { 1.0f, 1.0f },
                1.0f,
                { 1.0f, 0.0f, 0.0f }
            });

            vertices.push_back({
                { maxX, minY, minZ },
                { 0.0f, 0.0f },
                1.0f,
                { 1.0f, 0.0f, 0.0f }
            });

            vertices.push_back({
                { maxX, maxY, maxZ },
                { 1.0f, 1.0f },
                1.0f,
                { 1.0f, 0.0f, 0.0f }
            });

            vertices.push_back({
                { maxX, maxY, minZ },
                { 0.0f, 1.0f },
                1.0f,
                { 1.0f, 0.0f, 0.0f }
            });

            // Top face
            vertices.push_back({
                { minX, maxY, minZ },
                { 0.0f, 0.0f },
                1.0f,
                { 0.0f, 1.0f, 0.0f }
            });

            vertices.push_back({
                { maxX, maxY, minZ },
                { 1.0f, 0.0f },
                1.0f,
                { 0.0f, 1.0f, 0.0f }
            });

            vertices.push_back({
                { maxX, maxY, maxZ },
                { 1.0f, 1.0f },
                1.0f,
                { 0.0f, 1.0f, 0.0f }
            });

            vertices.push_back({
                { minX, maxY, minZ },
                { 0.0f, 0.0f },
                1.0f,
                { 0.0f, 1.0f, 0.0f }
            });

            vertices.push_back({
                { maxX, maxY, maxZ },
                { 1.0f, 1.0f },
                1.0f,
                { 0.0f, 1.0f, 0.0f }
            });

            vertices.push_back({
                { minX, maxY, maxZ },
                { 0.0f, 1.0f },
                1.0f,
                { 0.0f, 1.0f, 0.0f }
            });
        }
    }
    // Exit marker
    float mazeCenterX =
        static_cast<float>(maze.getWidth() - 1) / 2.0f;

    float mazeCenterZ =
        static_cast<float>(maze.getHeight() - 1) / 2.0f;

    float exitCenterX =
        static_cast<float>(exitX) - mazeCenterX;

    float exitCenterZ =
        static_cast<float>(exitY) - mazeCenterZ;

    float markerHalfSize = 0.2f;
    float markerMinX = exitCenterX - markerHalfSize;
    float markerMaxX = exitCenterX + markerHalfSize;
    float markerMinZ = exitCenterZ - markerHalfSize;
    float markerMaxZ = exitCenterZ + markerHalfSize;

    float markerMinY = 0.0f;
    float markerMaxY = 0.6f;

    const float markerTextureIndex = 3.0f;

    // Front
    vertices.push_back({
        { markerMinX, markerMinY, markerMinZ },
        { 0.0f, 0.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMaxX, markerMinY, markerMinZ },
        { 1.0f, 0.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMaxX, markerMaxY, markerMinZ },
        { 1.0f, 1.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMinX, markerMinY, markerMinZ },
        { 0.0f, 0.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMaxX, markerMaxY, markerMinZ },
        { 1.0f, 1.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMinX, markerMaxY, markerMinZ },
        { 0.0f, 1.0f },
        markerTextureIndex
    });

    // Back
    vertices.push_back({
        { markerMaxX, markerMinY, markerMaxZ },
        { 0.0f, 0.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMinX, markerMinY, markerMaxZ },
        { 1.0f, 0.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMinX, markerMaxY, markerMaxZ },
        { 1.0f, 1.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMaxX, markerMinY, markerMaxZ },
        { 0.0f, 0.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMinX, markerMaxY, markerMaxZ },
        { 1.0f, 1.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMaxX, markerMaxY, markerMaxZ },
        { 0.0f, 1.0f },
        markerTextureIndex
    });

    // Left
    vertices.push_back({
        { markerMinX, markerMinY, markerMaxZ },
        { 0.0f, 0.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMinX, markerMinY, markerMinZ },
        { 1.0f, 0.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMinX, markerMaxY, markerMinZ },
        { 1.0f, 1.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMinX, markerMinY, markerMaxZ },
        { 0.0f, 0.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMinX, markerMaxY, markerMinZ },
        { 1.0f, 1.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMinX, markerMaxY, markerMaxZ },
        { 0.0f, 1.0f },
        markerTextureIndex
    });

    // Right
    vertices.push_back({
        { markerMaxX, markerMinY, markerMinZ },
        { 0.0f, 0.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMaxX, markerMinY, markerMaxZ },
        { 1.0f, 0.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMaxX, markerMaxY, markerMaxZ },
        { 1.0f, 1.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMaxX, markerMinY, markerMinZ },
        { 0.0f, 0.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMaxX, markerMaxY, markerMaxZ },
        { 1.0f, 1.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMaxX, markerMaxY, markerMinZ },
        { 0.0f, 1.0f },
        markerTextureIndex
    });

    // Top
    vertices.push_back({
        { markerMinX, markerMaxY, markerMinZ },
        { 0.0f, 0.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMaxX, markerMaxY, markerMinZ },
        { 1.0f, 0.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMaxX, markerMaxY, markerMaxZ },
        { 1.0f, 1.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMinX, markerMaxY, markerMinZ },
        { 0.0f, 0.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMaxX, markerMaxY, markerMaxZ },
        { 1.0f, 1.0f },
        markerTextureIndex
    });

    vertices.push_back({
        { markerMinX, markerMaxY, markerMaxZ },
        { 0.0f, 1.0f },
        markerTextureIndex
    });

    return vertices;
}