#pragma once

#include <string>

struct TextureData
{
    int width = 0;
    int height = 0;
    int channels = 0;

    unsigned char* pixels = nullptr;
};

TextureData loadTexture(const std::string& filename);

void freeTexture(TextureData& texture);