#pragma once

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <filesystem.h>
#include <nds.h>
#include <NEMain.h>

class SecurityCamera
{
private:
    NE_Model *model;
    NE_Material *material;

    const float x = -22.5;
    const float y = 7.9;
    const float z = 16;

public:
    SecurityCamera();

    int Load();
    void FacePlayer(float playerX, float playerZ);
    void Draw();
    void Unload();
};