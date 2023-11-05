#pragma once

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <memory>
#include <algorithm>

#include <filesystem.h>
#include <nds.h>
#include <nf_lib.h>

#include "enums.h"
#include "sound.h"
#include "map.h"
#include "save.hpp"
#include "rumble.h"

void LoadBackground(const char name[]);
void DeleteBackground(const char name[]);

class Minigame
{
public:
    virtual void Load() = 0;
    virtual void Unload(Map* map) = 0;
    virtual void Update(volatile int frame, uint32 keys, Sound *sound, Map *map) = 0;
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
    void Unload(Map* map);
    void Update(volatile int frame, uint32 keys, Sound *sound, Map *map);
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
    void Unload(Map* map);
    void Update(volatile int frame, uint32 keys, Sound *sound, Map *map);
    bool IsComplete();
    MinigameResult GetResult(int framesTaken);
};

class MixMinigame : public Minigame
{
public:
private:
    const int PIPE_POS[2] = { -191, -78 };
    const int PIPE_COUNT = 8;
    const int HORIZONTAL_PIPE_COUNT = 5;
    const u32 PIPE_SPRITES[8] = { 16, 17, 18, 19, 20, 21, 22, 23 };

    const char PIPE_SPRITES_NAME[32] = "sprite/mixing_pipes";
    const char MIX_BACKGROUND_NAME[32] = "bg/mix_minigame";
    const u32 PIPE_OFFSET = 40;
    const int TARGET_PIPE_POS[2] = { 110, 57 };
    const int TARGET_POS_VARIENCE = 2;
    bool pipeInPlace = false;
    int pipeInPlaceFrames = 0;
    
    
    const u32 CRANK_SPRITES[2] = { 14, 15 };
    const int CRANK_POS[2][2] = { { 10, 55 }, { 125, 90 } };
    int crankRot[2] = { 0, 0 };
    int rotatingCrank = -1;
    
    int lastStylusPos[2] = {-1, -1};

public:
    MixMinigame();

    void Load();
    void Unload(Map* map);
    void Update(volatile int frame, uint32 keys, Sound *sound, Map *map);
    bool IsComplete();
    MinigameResult GetResult(int framesTaken);
};

class PipetteMinigame : public Minigame
{
private:
    const int SPRITE_HEIGHT = 32;

    const u32 PIPETTE_SPRITES = 15;
    const char PIPETTE_BACKGROUND_NAME[32] = "bg/pipette_minigame";
    const char PIPETTE_SPRITES_NAME[32] = "sprite/pipette";
    bool showingTubes = false;
    
    int lastStylusPos[2] = { -1, -1 };

    const int PIPETTE_SEGMENTS = 3;
    const int PIPETTE_SPRITE_IDS[3] = { 15, 16, 17 };
    int pipettePos[2] = { 5, 5 };

    const u32 DROPLET_SPRITE = 14;
    bool showDroplet = false;
    bool dropletDropping = false;
    bool dropletSplashing = false;
    int dropletFrames = 0;
    int dropletPos[2] = { 0, 0 };
    int dropletYVel = 0;
    const int DROPLET_TARGET_Y_CONTACT = 107;
    const int DROPLET_MISS_Y_CONTACT = 157;
    int dropletsMissed = 0;

    const int TUBE_COUNT = 3;
    const int TUBE_SPRITE_OFFSET = 20;
    const int TUBE_SEGMENTS = 4;
    const int TUBE_BASE_POS[2] = { 25, 32 };
    const int TUBE_SPACING = 70;

    int tubeProgress[3] = { 0, 0, 0 };
    const int TUBE_THRESHOLD = 8;
    int tubeBubbleAnimationFrames[3] = { 0, 0, 0 };

    void UpdateTubes(volatile int frame);

public:
    PipetteMinigame();

    void Load();
    void Unload(Map* map);
    void Update(volatile int frame, uint32 keys, Sound *sound, Map *map);
    bool IsComplete();
    MinigameResult GetResult(int framesTaken);
};

class PourMinigame : public Minigame
{
private:
    const int SPRITE_HEIGHT = 32;

    const u32 POUR_SPRITES = 15;
    const char POUR_BACKGROUND_NAME[32] = "bg/pour_minigame";
    const char POUR_SPRITES_NAME[32] = "sprite/pour";

    const u32 PUMP_SWITCH_SPRITE = 22;
    const int PUMP_SWITCH_POS[2] = { 195, 11 };

    const u32 DRAIN_SWITCH_SPRITE = 23;
    const int DRAIN_SWITCH_POS[2] = { -7, 60 };

    const int POURING_FLUID_COUNT = 6;
    const u32 POURING_FLUID_SPRITES[6] = { 24, 25, 26, 27, 28, 29 };
    const u32 PIPE_SPRITE = 30;
    const int PIPE_SPRITE_POS[2] = { 50, 0 };

    const float MAX_PUMP_RATE = 2.3f;
    const float MAX_DRAIN_RATE = 2.3f;
    float currentPumpRate = 0.0f;
    float targetPumpRate = 0.0f;
    float currentDrainRate = 0.0f;
    float targetDrainRate = 0.0f;

    const float MAX_LEVEL = 9.0f;
    const float TARGET_LEVEL = 6.5f;
    float currentLevel = 0.0f;
    
    const int TRAY_SPRITE_COUNT = 3;
    const int TRAY_SPRITES[3] = { 16, 17, 18 };
    const int FLUID_SPRITES[3] = { 19, 20, 21 };
    bool showingTray = false;
    bool showingSwitches = false;

    int selectedSwitch = -1;

    void UpdateTray();
    void UpdateSwitches(volatile int frame, Sound *sound);

public:
    PourMinigame();

    void Load();
    void Unload(Map* map);
    void Update(volatile int frame, uint32 keys, Sound *sound, Map *map);
    bool IsComplete();
    MinigameResult GetResult(int framesTaken);
};

class CrackMinigame : public Minigame
{
private:
    const int SPRITE_DIMS[2] = { 64, 32 };

    const u32 CRACK_SPRITE_BASE_ID = 15;
    const char CRACK_BACKGROUND_NAME[32] = "bg/crack_minigame";
    const char CRACK_SPRITES_NAME[32] = "sprite/crack";
    const int SPRITE_COUNTS[2] = { SCREEN_WIDTH / SPRITE_DIMS[0], 96 / SPRITE_DIMS[1] };
    const int CRACK_SPRITE_POS[2] = { 0, 40 };
    
    bool showingSprites = false;
    u32 spriteDamageTable[16][16];
    int spritePositions[16][16][2];

    void UpdateSprites(volatile int frame, Sound *sound, bool steelHammer);
    bool ProcessSprite(volatile int frame, int spriteId, int grid[2], Sound *sound, bool steelHammer);

public:
    CrackMinigame();

    void Load();
    void Unload(Map* map);
    void Update(volatile int frame, uint32 keys, Sound *sound, Map *map);
    bool IsComplete();
    MinigameResult GetResult(int framesTaken);
};