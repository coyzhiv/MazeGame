#version 450

layout(location = 0) in vec2 fragUV;
layout(location = 1) flat in float fragTextureIndex;
layout(location = 2) in vec3 fragNormal;

layout(set = 0, binding = 0) uniform sampler2D textures[3];

layout(location = 0) out vec4 outColor;

void main()
{
    vec3 normal = normalize(fragNormal);

    vec3 lightDirection =
        normalize(vec3(-0.5, -1.0, -0.3));

    float diffuse =
        max(dot(normal, -lightDirection), 0.0);

    float ambient = 0.7;

    float lighting =
        ambient + diffuse * 0.3;
    
    if (fragTextureIndex < 0.5)
    {
        outColor =
            texture(textures[0], fragUV) * lighting;
    }
    else if (fragTextureIndex < 1.5)
    {
        outColor =
            texture(textures[1], fragUV) * lighting;
    }
    else if (fragTextureIndex < 2.5)
    {
        outColor =
            texture(textures[2], fragUV) * lighting;
    }
    else
    {
        outColor = vec4(1.0, 1.0, 0.0, 1.0);
    }
}