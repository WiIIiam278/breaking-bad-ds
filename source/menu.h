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
#include "multiplayer.h"

class Menu
{
private:
    NE_Model *backdrop;
    NE_Material *backdropMaterial;

    NE_Model *logo;
    NE_Material *logoMaterial;

    NE_Model *text;
    NE_Material *textMaterial;

    MenuState state = MENU_LOGO;
    MenuSelection highlightedItem = NONE;

    const int MENU_BG_ID = 1;
    const char TITLE_BG_NAME[32] = "bg/title";
    const char MP_BG_NAME[32] = "bg/multiplayer";
    int currentBackground = 0; // 0 = none, 1 = bg/title, 2 = bg/multiplayer

    const float TARGET_X = 1;
    const float X_SPEED = 0.025;
    float x = TARGET_X - 10;
    const float Y = 4;
    const float Z = -2;

    const float TARGET_TEXT_X = TARGET_X - 1;
    float textX = TARGET_X - 16;

    const int START_SPRITE = 5;
    bool showStartSprite = true;

    const int BUTTONS[4] = {6, 7, 8, 9};
    const float MAIN_BUTTON_COORDS[4][2] = {{20, 10}, {105, 10}, {105, 55}, {20, 55}};
    const float GAME_BUTTON_COORDS[4][2] = {{20, 8}, {105, -10}, {60, 62}, {105, 25}};
    bool buttonsVisible = false;

    const KEYPAD_BITS SOUND_TEST_SEQUENCE[11] = {KEY_UP, KEY_UP, KEY_DOWN, KEY_DOWN,
                                                 KEY_LEFT, KEY_RIGHT, KEY_LEFT, KEY_RIGHT,
                                                 KEY_B, KEY_A, KEY_START};
    int currentSequenceIndex = 0;
    int currentSoundTestTrack = 1;

    const int MP_STATUS_SPRITE = 11;
    int mpCurrentStatus = -1;
    bool mpCreatingRoom;
    bool mpShowingStatus;

public:
    Menu();

    int Load();
    void LoadSplashSprite();
    void ShowButtons();
    void ShowBackground();
    void Update(volatile int frame, Sound *sound);
    void Draw(volatile int frame);
    void SetState(MenuState newState, Sound *sound);
    void PositionLogo();
    void StartMultiplayer(bool mpCreatingRoom);
    void UpdateMultiplayer();
    void ShowMultiplayerStatus(bool showSprite);
    MenuSelection HandleInput(Sound *sound);
    bool IsTouchInBox(const float coords[2], int boxHeight, touchPosition touch);
    MenuSelection CheckSelection(MenuSelection tappedBox);
    void Unload(Sound *sound);
};

extern "C" {
    void nifiInit();
    void joinMultiplayer(bool hostRoom);
    void tickMultiplayer();
    void disableMultiplayer();
    int getMultiplayerStatus();
    Client *getOpponent();
}