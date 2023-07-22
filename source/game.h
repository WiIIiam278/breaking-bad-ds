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
    const bool debugFlag = false;
    volatile int frame;
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
    Speaker currentSpeaker = GALE;
    char currentScript[128][128];
    int currentScriptLength = 0;
    int currentLineIndex = 0;
    int currentLineStartFrame = 0;
    int currentSpeakerAnimation = 0;

    // Tutorial
    bool isTutorial = false;
    int tutorialProgress = 0;

    // Game
    int timeLimit = 180;
    int batchQuota = 10;

    // Minigames
    ValveMinigame valveMinigame;
    Minigame *currentMinigame = NULL;

public:
    Game();

    // Setup
    void WaitLoop();
    void Prepare3DGraphics();
    void Prepare2DGraphics();

    // Transition Control
    void Transition(bool fadeIn, int duration);
    void UpdateTransition(volatile int frame);

    // Camera
    void UpdateCamera(volatile int frame);
    void TranslateCamera(float x, float y, float z);

    // Start game
    void StartGame(bool tutorialGame, int timeLimit, int batchQuota);
    void LoadLabScene();
    void UnLoadLabScene();

    // Start title screen
    void StartMenuScreen();
    void LoadLogoScene();
    void UnLoadLogoScene();
    void UpdateMenuScreen(volatile int frame);

    // Dialogue
    void SetDialogue(Speaker speaker, const char script[][128], int scriptLength, int startFrame);
    void UpdateDialogue(volatile int frame, uint32 keys);
    void ClearDialogue();

    // Minigame stuff
    void StartMinigame(Tile tile);
    void DeleteMinigame();

    // Main tick logic
    void Update(volatile int frame);
    void Render(volatile int frame);
};