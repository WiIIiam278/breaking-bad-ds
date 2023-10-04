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
#include "layouts.h"
#include "minerals.h"
#include "save.h"
#include "transitions.h"
#include "rumble.h"

class Menu
{
private:
    NE_Model *skybox;
    NE_Material *skyboxMaterial;
    const float SKYPOS_POS[3] = {0, 8, 0};

    NE_Model *logo;
    NE_Material *logoMaterial;

    NE_Model *text;
    NE_Material *textMaterial;

    MenuState state = MENU_LOGO;
    MenuSelection highlightedItem = NONE;
    int menuCaret[2] = {0, 0};

    const int MENU_BG_ID = 1;
    char *currentBgName;
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

    const char MENU_MISC_SPRITE_NAME[32] = "sprite/menu_misc";
    const char MENU_BUTTONS_SPRITE_NAME[32] = "sprite/menu_buttons";
    const char MENU_TEXT_SPRITE_NAME[32] = "sprite/menu_text";
    const int MENU_SPRITE_SIZE[2] = {64, 32};

    const int BUTTON_GFX_ID = 2;
    const int BUTTON_SPRITE_COUNT = 16;
    const int BUTTON_SPRITES_BASE = 7;
    
    const int TEXT_GFX_ID = 3;
    const int TEXT_SPRITES[4] = {3, 4, 5, 6};
    
    const int TITLE_SPRITE = 1;
    Layout *currentLayout = nullptr;

    // Sound test
    int currentSoundTestTrack = 1;

    // Minerals
    const int MINERAL_GRID_BASE_POS[2] = {5, 41};
    const int MINERAL_GAP = 8;
    const int MINERALS_PER_ROW = MINERAL_COUNT / 3;
    const int MINERAL_SPRITE_BASE = 32;
    const int MINERAL_CURSOR_SPRITE = 31;
    const int HANK_EYES_POS[2] = {173, 52};
    const int HANK_EYES_SPRITE = 30;
    bool showingMinerals = false;
    int currentlySelectedMineral = -1;
    int hankLastBlink = 0;

    // Multiplayer status
    const int MP_STATUS_SPRITE = 11;
    int mpCurrentStatus = -1;
    bool mpCreatingRoom;
    bool mpShowingStatus;

public:
    Menu();

    int Load();
    void LoadInterfaceSprites();
    void UnloadInterfaceSprites();
    void ShowLayout();
    void UpdateLayout(volatile int frame);
    void ShowBackground();
    void Update(volatile int frame, Sound *sound, SaveFile *saveFile);
    void Draw(volatile int frame);
    void SetState(MenuState newState, volatile int frame, Sound *sound, SaveFile *saveFile);
    void PositionLogo();
    void StartMultiplayer(bool mpCreatingRoom);
    void UpdateMultiplayer();
    void ShowMultiplayerStatus(bool showSprite);
    void ShowMinerals(bool showSprites, SaveFile *saveFile);
    void UpdateMinerals(volatile int frame, Sound *sound, SaveFile *saveFile);
    MenuSelection HandleInput(volatile int frame, Sound *sound, SaveFile *saveFile);
    MenuSelection HandleLayoutInput(volatile int frame, Sound *sound, SaveFile *saveFile, touchPosition touch);
    MenuSelection HandleClick(MenuSelection clicked, volatile int frame, Sound *sound, SaveFile *saveFile);
    bool IsTouchInBox(const int coords[2], const int boxDimensions[2], touchPosition touch);
    MenuSelection CheckSelection(MenuSelection tappedBox);
    void Unload(volatile int frame, Sound *sound, SaveFile *saveFile);
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