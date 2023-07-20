#pragma once

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <filesystem.h>
#include <nds.h>
#include <NEMain.h>
#include <nf_lib.h>

class Logo
{
private:
    NE_Model *backdrop;
    NE_Material *backdropMaterial;

    NE_Model *model;
    NE_Material *material;

    NE_Model *textPlane;
    NE_Material *textMaterial;

    const float TARGET_X = 1;
    const float X_SPEED = 0.025;
    float x = TARGET_X - 10;
    const float y = 4;
    const float z = -2;

    const float TARGET_TEXT_X = TARGET_X - 1;
    float textX = TARGET_X - 16;

    const int START_SPRITE = 5;
    bool showStartSprite = true;

public:
    Logo();

    int Load();
    void Update(volatile int frame);
    void Draw(volatile int frame);
    void Unload();
};