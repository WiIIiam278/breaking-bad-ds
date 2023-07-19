#pragma once

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <filesystem.h>
#include <nds.h>
#include <NEMain.h>

class Logo
{
private:
    NE_Model *model;
    NE_Material *material;

    const float X_SPEED = 0.025;
    float x = -15;
    const float y = 5;
    const float z = 0;

public:
    Logo();

    int Load();
    void Update(volatile int frame);
    void Draw();
    void Unload();
};