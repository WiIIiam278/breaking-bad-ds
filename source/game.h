#pragma once

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <memory>

#include <filesystem.h>
#include <nds.h>
#include <NEMain.h>
#include <nf_lib.h>

#include "menu.h"
#include "map.h"
#include "player.h"
#include "minigames.h"
#include "enums.h"
#include "dialogue.h"
#include "debug.h"
#include "sound.h"
#include "multiplayer.h"

using namespace std;

class Game
{
private:
    volatile int frame = 0;
    bool debugFlag = true;
    Mode mode = MOVE;
    bool isQuitting = false;

    // Main game settings
    GameType gameType;
    Menu menu;
    Map map;
    Player player;
    Sound sound;
    const u32 CLEAR_COLOR = NE_Black;

    // Multiplayer
    Player *player2 = NULL;
    int player2BatchProgress = 0;
    int player2batchesComplete = 0;

    // Transition
    bool isTransitioning = false;
    bool isFadingIn = false;
    int transitionDuration = 0;
    int transitionStartFrame = 0;
    
    // Camera
    const float CAMERA_SPEED = 0.025;
    const float BASE_TITLE_CAMERA_POS[3] = {-10, 8, 0};
    const float BASE_MOVE_CAMERA_POS[3] = {-11.5, 12.5, -4};
    float cameraX = BASE_TITLE_CAMERA_POS[0];
    float cameraY = BASE_TITLE_CAMERA_POS[1];
    float cameraZ = BASE_TITLE_CAMERA_POS[2];
    float cameraTx = cameraX;
    float cameraTy = cameraY;
    float cameraTz = cameraZ;
    NE_Camera *camera;

    // Dialogue
    Dialogue dialogue;

    // Tutorial
    int tutorialProgress = 0;
    const float HUD_BATCH_PROGRESS_MARKER_COORDS[6][2] = {{-24.8, 1.9}, {-4, 6.8}, {-14.4, 4.2}, {-4, 16.2}, {-22.1, 9.1}, {-22.1, 16.2}};

    // Game
    int timeLimit = -1;
    int batchQuota = 1;
    int batchesComplete = 0;
    int currentBatchProgress = 0;
    int gameOverFrame = 0;
    int batchPurity = 100;
    volatile int idleFrames = 0;
    
    // HUD
    const int HUD_MAP_ICONS = 6;
    const int HUD_CHECKBOXES = 7;
    const int HUD_NUMBERS = 8;
    const int HUD_MAP_PLAYER_SPRITE = 6;
    const int HUD_MAP_PLAYER2_SPRITE = 7;
    const int HUD_MAP_MARKER_SPRITE = 8;
    const int HUD_CHECKBOX_SPRITES[6] = {9, 10, 11, 12, 13, 14};
    const int HUD_TIMER_SPRITES[3] = {20, 21, 22};
    const int HUD_PURITY_SPRITES[3] = {23, 24, 25};
    const int HUD_QUOTA_SPRITES[4] = {26, 27, 28, 29};
    const float HUD_CHECKBOXES_COORDS[2] = {103, 55};
    const int HUD_CHECKBOX_COUNT = 6;
    const int HUD_CHECKBOX_Y_SPACING = 19;
    const float HUD_MAP_ORIGIN_COORDS[2] = {136, 102};
    const float HUD_TIMER_COORDS[2] = {178, 125};
    const float HUD_PURITY_COORDS[2] = {178, 144};
    const float HUD_QUOTA_COORDS[2] = {176, 163};
    bool hudVisible = false;
    
    // Minigames
    const int QUALITY_INDICATOR_SPRITE = 5;
    int showingIndicatorFor = 0;
    int inMinigameFor = 0;
    Minigame *currentMinigame = NULL;

    // Backgrounds
    const int HUD_BG = 1;
    const char SP_HUD_BG_NAME[32] = "bg/hud";
    const char MP_HUD_BG_NAME[32] = "bg/multiplayer_hud";
    const int PAUSED_BG = 2;
    const char PAUSED_BG_NAME[32] = "bg/paused";

public:
    Game();

    // Setup
    void WaitLoop();
    void Prepare3DGraphics();
    void Prepare2DGraphics();

    // Transition Control
    void Transition(bool fadeIn, int duration);
    void UpdateTransition();

    // Pause menu
    void TogglePauseMenu();

    // HUD
    void ToggleHud(bool show);
    void UpdateHud();

    // Quit to title
    void QuitToTitle();

    // Dialogue
    void StartDialogue(ScriptId scriptId);
    void EndDialogue();
    void CheckTutorials();

    // Camera
    void UpdateCamera();
    void TranslateCamera(float x, float y, float z);

    // Start game
    void StartGame(GameType gameType, int timeLimit, int batchQuota);
    void LoadLabScene();
    void UnLoadLabScene();

    // Multiplayer

    // Start title screen
    void StartMenuScreen(bool debugMode);
    void LoadLogoScene();
    void UnLoadLogoScene();
    void UpdateMenuScreen();

    // Minigame stuff
    void StartMinigame(Tile tile);
    void DeleteMinigame();
    void ShowMinigameResult(MinigameResult indicator, int frames);

    // Game Over
    void StartGameOver();
    void UpdateGameOver();

    // Main tick logic
    void Tick();
    void Update();
    void Render();
};

extern "C" {
    Client* getOpponent();
    Client* getLocalClient();
    void tickMultiplayer();
    void disableMultiplayer();
    int getMultiplayerStatus();
    bool isHostClient();
}