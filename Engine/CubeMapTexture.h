#pragma once

#include "Texture.h"

class CubeMapTexture : public Texture {
public:
    CubeMapTexture();
    ~CubeMapTexture();

    void Load(const std::wstring& path) override;
};