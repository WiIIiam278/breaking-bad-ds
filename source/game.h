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
#include "scripts.h"
#include "debug.h"
#include "sound.h"

using namespace std;

class Game
{
private:
    volatile int frame = 0;
    bool debugFlag = true;
    Mode mode = MOVE;

    // 3D Gameplay
    Menu menu;
    Map map;
    Player player;
    const u32 CLEAR_COLOR = RGB15(3, 3, 3);

    // Sound
    Sound sound;

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
    const int CHARACTERS_PER_DIALOGUE_LINE = 29;
    Speaker currentSpeaker = GALE;
    char currentScript[128][128];
    int currentScriptLength = 0;
    int currentLineIndex = 0;
    volatile int currentLineStartFrame = 0;
    int currentSpeakerAnimation = 0;

    // Tutorial
    bool isTutorial = false;
    int tutorialProgress = 0;
    const float HUD_BATCH_PROGRESS_MARKER_COORDS[6][2] = {{-24.8, 1.9}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};

    // Game
    int timeLimit = -1;
    int batchQuota = 1;
    int currentBatchProgress = 0;
    int gameOverFrame = 0;
    
    // HUD
    const int HUD_MAP_ICONS = 6;
    const int HUD_MAP_PLAYER_SPRITE = 6;
    const int HUD_MAP_MARKER_SPRITE = 7;
    const float HUD_MAP_ORIGIN_COORDS[2] = {136, 102};
    bool hudVisible = false;
    
    // Minigames
    const int QUALITY_INDICATOR_SPRITE = 12;
    int showingIndicatorFor = 0;
    int inMinigameFor = 0;
    ValveMinigame valveMinigame;
    Minigame *currentMinigame = NULL;

    // Backgrounds
    const int LAB_BG = 3;
    const char LAB_BG_NAME[32] = "bg/lab";
    const int HUD_BG = 1;
    const char HUD_BG_NAME[32] = "bg/hud";

public:
    Game();

    // Setup
    void WaitLoop();
    void Prepare3DGraphics();
    void Prepare2DGraphics();

    // Transition Control
    void Transition(bool fadeIn, int duration);
    void UpdateTransition();

    // HUD
    void ToggleHud(bool show);
    void UpdateHud();

    // Camera
    void UpdateCamera();
    void TranslateCamera(float x, float y, float z);

    // Start game
    void StartGame(bool tutorialGame, int timeLimit, int batchQuota);
    void LoadLabScene();
    void UnLoadLabScene();

    // Start title screen
    void StartMenuScreen(bool debugMode);
    void LoadLogoScene();
    void UnLoadLogoScene();
    void UpdateMenuScreen();

    // Dialogue
    void SetDialogue(Speaker speaker, const char script[][128], int scriptLength, int startFrame);
    void UpdateDialogue(uint32 keys);
    void ClearDialogue();

    // Minigame stuff
    void StartMinigame(Tile tile);
    void DeleteMinigame();
    void ShowQualityIcon(MinigameResult indicator, int frames);

    // Game Over
    void StartGameOver();
    void UpdateGameOver();

    // Main tick logic
    void Tick();
    void Update();
    void Render();
};