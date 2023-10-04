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
#include "rumble.h"

class Player
{
private:
    float maxSpeed = 0.125f;
    const float ACCELERATION = 7.0f;
    float currentSpeed[2] = { 0, 0 };
    const float TILE_SIZE = (maxSpeed / ACCELERATION) + 0.15f;

    volatile float lyingDownFrames = 0;

    NE_Model *model;
    NE_Material *material;
    NE_Animation *animation[2];

public:
    bool isPlayer2 = false;
    Character character = CHAR_WALT;
    bool canMove = true;
    Direction facing = DOWN;
    bool walking = false;
    bool lyingDown = false;
    bool inDialogue = false;

    int tileX;
    int tileZ;
    int targetX;
    int targetZ;

    float x;
    float y;
    float z;
    float rotation;

    Player();

    int Load(Character character, NE_Animation *animations[2]);
    void ResetPosition(bool airJordans);
    void Move(Map& map);
    void Update(volatile int frame);
    void Translate(float x, float y, float z);
    Tile GetTile(Map& map);
    void HandleInput(uint32 keys);
    void SetLyingDown(bool lyingDown);
    void DrawShadow(float radius, u32 color);
    void DrawTriangle(float *vertexes[], float offset[3], u32 color);
    void Draw(u32 outline);
    void Unload();
};