#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Texture.h"

#include <stdexcept>

TextureData loadTexture(const std::string& filename)
{
    TextureData texture{};

    texture.pixels = stbi_load(
        filename.c_str(),
        &texture.width,
        &texture.height,
        &texture.channels,
        STBI_rgb_alpha
    );

    if (!texture.pixels)
    {
        throw std::runtime_error(
            "Failed to load texture: " + filename
        );
    }

    texture.channels = 4;

    return texture;
}

void freeTexture(TextureData& texture)
{
    if (texture.pixels)
    {
        stbi_image_free(texture.pixels);
        texture.pixels = nullptr;
    }
}