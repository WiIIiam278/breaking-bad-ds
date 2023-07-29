#pragma once

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <filesystem.h>
#include <nds.h>
#include <NEMain.h>
#include <nf_lib.h>

#include "map.h"
#include "enums.h"

class Player
{
private:
    const float SPEED = 0.125;
    const float TILE_SIZE = 0.05;

    volatile float lyingDownFrames = 0;

    NE_Model *model;
    NE_Material *material;

public:
    bool canMove = true;
    Direction facing = DOWN;
    bool walking = false;
    bool lyingDown = false;

    int tileX;
    int tileZ;
    int targetX;
    int targetZ;

    float x;
    float y;
    float z;
    float rotation;

    Player();

    int Load();
    void Move(Map& map);
    void Update(volatile int frame);
    void Translate(float x, float y, float z);
    Tile GetPlayerTile(Map& map);
    void PrintCoords(Map& map);
    void HandleInput(uint32 keys);
    void SetLyingDown(bool lyingDown);
    void DrawShadow(float radius, u32 color);
    void DrawTriangle(float *vertexes[], float offset[3], u32 color);
    void Draw();
    void Unload();
};