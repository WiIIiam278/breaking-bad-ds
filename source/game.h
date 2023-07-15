#pragma once

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <filesystem.h>
#include <nds.h>
#include <NEMain.h>
#include <nf_lib.h>

#include "map.h"
#include "player.h"
#include "enums.h"
#include "scripts.h"

using namespace std;

class Game
{
private:
    volatile int frame;
    Mode mode = MOVE;

    // 3D Gameplay
    Map map;
    Player player;
    
    // Camera
    const float CAMERA_SPEED = 0.025;
    NE_Camera *camera;
    float cameraX = -11.5;
    float cameraY = 12.5;
    float cameraZ = -4;
    float cameraTx = cameraX;
    float cameraTy = cameraY;
    float cameraTz = cameraZ;
    
    // Dialogue
    Speaker currentSpeaker = GALE;
    char currentScript[128][128];
    int currentScriptLength = 0;
    int currentLineIndex = 0;
    int currentLineStartFrame = 0;
    int currentSpeakerAnimation = 0;

    bool debugFlag = true;

public:
    Game();

    // Setup
    void WaitLoop();
    void Prepare3DGraphics();
    void Prepare2DGraphics();

    // Camera
    NE_Camera *SetupCamera();
    void UpdateCamera(volatile int frame);
    void TranslateCamera(float x, float y, float z);
    void PrintCameraCoords();

    // Dialogue
    void SetDialogue(Speaker speaker, const char script[][128], int scriptLength, int startFrame);
    void UpdateDialogue(volatile int frame, uint32 keys);
    void ClearDialogue();

    // Main tick logic
    void Update(volatile int frame);
    void Render(volatile int frame);
};