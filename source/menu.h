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

struct Button
{
    const int buttonSprite;
    const int dimensions[2];
    const int textSprite;
    const MenuSelection selection;
};

struct Layout
{
    const Button buttons[4];
    const int buttonCoords[4][2];
    const int titleSprite;
    const int titleCoords[2];
    const MenuSelection caretMap[3][2];
};

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
    int menuCaret[2] = {0, 0};

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

    const int START_SPRITE = 2;
    bool showStartSprite = true;

    const int BUTTON_SPRITE_COUNT = 16;
    const int BUTTON_SPRITES[16] = {7, 8, 9, 10, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};
    const int BUTTON_TEXT_COUNT = 4;
    const int BUTTON_TEXT[4] = {3, 4, 5, 6};
    const int TITLE_SPRITE = 1;
    const Layout LAYOUTS[2] =
        {
            {{
                 {3, {90, 58}, 11, OPEN_GAME_MENU},
                 {0, {90, 36}, 12, START_TUTORIAL},
                 {1, {90, 26}, 10, BACK_TO_TITLE},
                 {2, {90, 26}, 13, TOGGLE_RUMBLE},
             },
             {
                 {17, 55},
                 {112, 39},
                 {65, 112},
                 {112, 75},
             },
             9,
             {98, 15},
             {
                 {OPEN_GAME_MENU, START_TUTORIAL},
                 {OPEN_GAME_MENU, TOGGLE_RUMBLE},
                 {BACK_TO_TITLE, BACK_TO_TITLE},
             }},
            {{
                 {3, {90, 58}, 8, START_1P_GAME},
                 {2, {90, 26}, 14, OPEN_ROOM},
                 {1, {90, 26}, 10, BACK_TO_MAIN_MENU},
                 {2, {90, 26}, 15, SEARCH_FOR_ROOMS},
             },
             {
                 {17, 63},
                 {112, 31},
                 {112, 97},
                 {112, 62},
             },
             11,
             {98, 15},
             {
                 {START_1P_GAME, OPEN_ROOM},
                 {START_1P_GAME, SEARCH_FOR_ROOMS},
                 {START_1P_GAME, BACK_TO_MAIN_MENU},
             }}};
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
    void ShowLayout();
    void UpdateLayout(volatile int frame);
    void ShowBackground();
    void Update(volatile int frame, Sound *sound);
    void Draw(volatile int frame);
    void SetState(MenuState newState, Sound *sound);
    void PositionLogo();
    void StartMultiplayer(bool mpCreatingRoom);
    void UpdateMultiplayer();
    void ShowMultiplayerStatus(bool showSprite);
    MenuSelection HandleInput(Sound *sound);
    MenuSelection HandleClick(MenuSelection clicked, Sound *sound);
    bool IsTouchInBox(const int coords[2], const int boxDimensions[2], touchPosition touch);
    MenuSelection CheckSelection(MenuSelection tappedBox);
    void Unload(Sound *sound);
};

extern "C"
{
    void nifiInit();
    void joinMultiplayer(bool hostRoom);
    void tickMultiplayer();
    void disableMultiplayer();
    int getMultiplayerStatus();
    Client *getOpponent();
};