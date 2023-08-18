#pragma once

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <filesystem.h>
#include <nds.h>
#include <NEMain.h>

#include "enums.h"
#include "security_camera.h"

class Map
{
private:
    const int MODEL_COUNT = 3;
    const char *modelPaths[3] = {"model/superlab.dl", "model/superlab_mixer.dl", "model/superlab_pipe.dl"};
    NE_Model *models[3];
    u32 mixerAlpha = 31;
    float pipePos[2] = { -191, -78 };
    
    NE_Material *material;
    SecurityCamera securityCamera;
    
    const Tile Tiles[9][10] = {
        {FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, MINIGAME_VALVE},
        {WALL, FLOOR, FLOOR, FLOOR, FLOOR, MINIGAME_MIX, FLOOR, FLOOR, FLOOR, WALL},
        {WALL, MINIGAME_PESTLE, FLOOR, WALL, WALL, WALL, FLOOR, FLOOR, FLOOR, WALL},
        {WALL, FLOOR, FLOOR, WALL, WALL, WALL, WALL, FLOOR, MINIGAME_POUR, WALL}, 
        {WALL, FLOOR, FLOOR, WALL, WALL, WALL, WALL, FLOOR, FLOOR, WALL},
        {WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, WALL},
        {WALL, MINIGAME_PIPETTE, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, MINIGAME_CRACK, WALL},
        {WALL, WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, WALL, WALL, WALL},
        {WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}};

public:
    Map();

    int Load();
    Tile GetTileAt(int x, int z);
    void RotateSecurityCamera(float playerX, float playerZ);
    void AdjustPipe(float pipeX, float pipeZ);
    void Draw(bool playerObscured);
    void Unload();
};