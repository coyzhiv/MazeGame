#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in float inTextureIndex;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec2 fragUV;
layout(location = 1) flat out float fragTextureIndex;
layout(location = 2) out vec3 fragNormal;

layout(set = 0, binding = 1) uniform CameraUBO
{
    mat4 view;
    mat4 projection;
    float time;
    float victoryTime;
} camera;

void main()
{
    vec3 position = inPosition;

    if (inTextureIndex > 2.5)
    {
        position.y += sin(camera.time * 3.0) * 0.1;

        if (camera.victoryTime > 0.0)
        {
            float progress =
                min(camera.victoryTime / 1.0, 1.0);

            float scale =
                1.0 - progress;
            
            if (scale <= 0.01)
            {
                position = vec3(0.0, -100.0, 0.0);
            }

            vec3 center = vec3(
                position.x,
                0.3,
                position.z
            );

            position =
                center +
                (position - center) * scale;
        }
    }

    fragNormal = inNormal;

    gl_Position =
        camera.projection *
        camera.view *
        vec4(position, 1.0);
    
    fragUV = inUV;
    fragTextureIndex = inTextureIndex;
}