#pragma once

#include "Texture.h"

class CubeMapTexture : public Texture {
public:
    CubeMapTexture();
    ~CubeMapTexture();

    virtual void Load(const std::wstring& path);
};