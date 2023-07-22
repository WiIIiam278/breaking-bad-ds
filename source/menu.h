#pragma once

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <filesystem.h>
#include <nds.h>
#include <NEMain.h>
#include <nf_lib.h>

#include "enums.h"
#include "sound.h"

class Menu
{
private:
    NE_Model *backdrop;
    NE_Material *backdropMaterial;

    NE_Model *logo;
    NE_Material *logoMaterial;

    NE_Model *text;
    NE_Material *textMaterial;

    MenuState state = LOGO;
    MenuSelection highlightedItem = NONE;

    const int TITLE_BG = 2;
    const char TITLE_BG_NAME[32] = "bg/title";

    const float TARGET_X = 1;
    const float X_SPEED = 0.025;
    float x = TARGET_X - 10;
    const float y = 4;
    const float z = -2;

    const float TARGET_TEXT_X = TARGET_X - 1;
    float textX = TARGET_X - 16;

    const int START_SPRITE = 5;
    bool showStartSprite = true;

    const int BUTTONS[4] = {6, 7, 8, 9};
    const float BUTTON_COORDS[4][2] = {{20, 10}, {105, 10}, {105, 55}, {20, 55}};

public:
    Menu();

    int Load(Sound *sound);
    void LoadSplashSprite();
    void LoadButtons();
    void Update(volatile int frame, Sound *sound);
    void Draw(volatile int frame);
    void SetState(MenuState newState, Sound *sound);
    MenuSelection HandleInput(Sound *sound);
    bool IsTouchInBox(const float coords[2], int boxHeight, touchPosition touch);
    MenuSelection CheckSelection(MenuSelection tappedBox);
    void Unload(Sound *sound);
};