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

using namespace std;

class Game
{
private:
    volatile int frame;
    Mode mode = MOVE;

    // 3D Gameplay
    Map map;
    Player player;
    NE_Camera *camera;
    
    // Dialogue
    Speaker currentSpeaker = GALE;
    char currentLine[128];
    int currentLineStartFrame = 0;
    int currentSpeakerAnimation = 0;

    bool debugFlag = true;

public:
    Game();

    // Setup
    void WaitLoop();
    void Prepare3DGraphics();
    void Prepare2DGraphics();

    // Dialogue
    void SetDialogue(Speaker speaker, char line[128], int startFrame);
    void UpdateDialogue(volatile int frame);
    void ClearDialogue();

    // Main tick logic
    void Update(volatile int frame);
    void Render(volatile int frame);
};