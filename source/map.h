#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <string>

#include <filesystem.h>
#include <nds.h>
#include <NEMain.h>

#include "enums.h"
#include "security_camera.h"

class Map
{
private:
    NE_Model *Model;
    NE_Material *Material;
    SecurityCamera Camera;

    const Tile Tiles[9][9] = {
        {FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR},
        {WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR},
        {WALL, FLOOR, FLOOR, WALL, WALL, WALL, FLOOR, FLOOR, FLOOR},
        {WALL, FLOOR, FLOOR, WALL, WALL, WALL, WALL, FLOOR, FLOOR},
        {WALL, FLOOR, FLOOR, WALL, WALL, WALL, WALL, FLOOR, FLOOR},
        {WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR},
        {WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR},
        {WALL, WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR},
        {WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}};

public:
    Map();

    int Load();
    Tile GetTileAt(int x, int z);
    void UpdateCamera(float playerX, float playerZ);
    void Draw();
};