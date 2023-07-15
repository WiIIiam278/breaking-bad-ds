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
    NE_Model *model;
    NE_Material *material;
    SecurityCamera securityCamera;

    const Tile Tiles[9][10] = {
        {FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR},
        {WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, WALL},
        {WALL, FLOOR, FLOOR, WALL, WALL, WALL, FLOOR, FLOOR, FLOOR, WALL},
        {WALL, FLOOR, FLOOR, WALL, WALL, WALL, WALL, FLOOR, FLOOR, WALL},
        {WALL, FLOOR, FLOOR, WALL, WALL, WALL, WALL, FLOOR, FLOOR, WALL},
        {WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, WALL},
        {WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, WALL},
        {WALL, WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, WALL, WALL, WALL},
        {WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}};

public:
    Map();

    int Load();
    Tile GetTileAt(int x, int z);
    void RotateSecurityCamera(float playerX, float playerZ);
    void Draw();
};