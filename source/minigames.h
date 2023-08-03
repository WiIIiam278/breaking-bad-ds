#pragma once

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <memory>

#include <filesystem.h>
#include <nds.h>
#include <nf_lib.h>

#include "enums.h"
#include "sound.h"

void LoadBackground(const char name[]);
void DeleteBackground(const char name[]);

class Minigame
{
public:
    virtual void Load() = 0;
    virtual void Unload() = 0;
    virtual void Update(volatile int frame, uint32 keys, Sound *sound) = 0;
    virtual bool IsComplete() = 0;
    virtual MinigameResult GetResult(int framesTaken) = 0;

    virtual ~Minigame() {}
};

class ValveMinigame : public Minigame
{
private:
    const int VALVE_ROTATION_SPEED = 4;

    const u32 VALVE_SPRITE = 15;
    const char VALVE_SPRITE_NAME[32] = "sprite/valve";
    const char VALVE_BACKGROUND_NAME[32] = "bg/valve_minigame";

    uint32 lastStylusPos[2] = {0, 0};

    int rotation = 0;
    const int TARGET_ROTATION = 2048;

public:
    ValveMinigame();

    void Load();
    void Unload();
    void Update(volatile int frame, uint32 keys, Sound *sound);
    bool IsComplete();
    MinigameResult GetResult(int framesTaken);
};

class PestleMinigame : public Minigame
{
private:
    const u32 PESTLE_SPRITE = 15;
    const u32 SOLUTE_SPRITES[3] = { 16, 17, 18 };
    const char PESTLE_SPRITES_NAME[32] = "sprite/pestle";
    const char PESTLE_BACKGROUND_NAME[32] = "bg/pestle_minigame";
    int lastSfxPlay = 0;
        
    const int SOLUTE_MAX_DAMAGE = 12;
    int soluteDamage[3] = { 0, 0, 0 };
    u16 solutePos[3][2] = { { 0, 0 }, { 0, 0 }, { 0, 0 } };
    int soluteRot[3] = { 0, 0, 0 };

    const int SWING_LENGTH = 10;
    const u16 VELOCITY_THRESHOLD = 800;
    const u16 PESTLE_PROXIMITY = 32;
    const int SFX_DURATION = 70;
    int pestleRotation = 0;
    u16 pestlePos[2] = { 0, 0 };
    u16 lastPestlePos[2] = { 0, 0 };
    int lastVelocityCheckFrame = 0;
    u16 velocity = 0;
    
public:
    PestleMinigame();

    void Load();
    void Unload();
    void Update(volatile int frame, uint32 keys, Sound *sound);
    bool IsComplete();
    MinigameResult GetResult(int framesTaken);
};

class MixMinigame : public Minigame
{
private:
    const int PIPE_COUNT = 8;
    const int HORIZONTAL_PIPE_COUNT = 5;
    const u32 PIPE_SPRITES[8] = { 16, 17, 18, 19, 20, 21, 22, 23 };

    const char PIPE_SPRITES_NAME[32] = "sprite/mixing_pipes";
    const char MIX_BACKGROUND_NAME[32] = "bg/mix_minigame";
    const u32 PIPE_OFFSET = 40;
    int pipePos[2] = { 0, 0 };
    
    
    const u32 CRANK_SPRITES[2] = { 24, 25 };
    const int CRANK_POS[2][2] = { { 10, 55 }, { 125, 90 } };
    int crankRot[2] = { 0, 0 };
    int rotatingCrank = -1;
    
    u16 lastStylusPos[2] = {0, 0};

public:
    MixMinigame();

    void Load();
    void Unload();
    void Update(volatile int frame, uint32 keys, Sound *sound);
    bool IsComplete();
    MinigameResult GetResult(int framesTaken);
};

class PipetteMinigame : public Minigame
{
private:

public:
    PipetteMinigame();

    void Load();
    void Unload();
    void Update(volatile int frame, uint32 keys, Sound *sound);
    bool IsComplete();
    MinigameResult GetResult(int framesTaken);
};

class PourMinigame : public Minigame
{
private:

public:
    PourMinigame();

    void Load();
    void Unload();
    void Update(volatile int frame, uint32 keys, Sound *sound);
    bool IsComplete();
    MinigameResult GetResult(int framesTaken);
};

class CrackMinigame : public Minigame
{
private:

public:
    CrackMinigame();

    void Load();
    void Unload();
    void Update(volatile int frame, uint32 keys, Sound *sound);
    bool IsComplete();
    MinigameResult GetResult(int framesTaken);
};