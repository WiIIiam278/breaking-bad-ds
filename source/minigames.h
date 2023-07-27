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
    virtual void Delete() = 0;
    virtual void Update(volatile int frame, uint32 keys, Sound *sound) = 0;
    virtual bool IsComplete() = 0;
    virtual MinigameResult GetResult(int framesTaken) = 0;
    virtual bool IsForCurrentBatch(int currentBatchProgress) = 0;
};

class ValveMinigame : public Minigame
{
private:
    const int VALVE_ROTATION_SPEED = 4;

    const u32 VALVE_SPRITE = 15;
    const char VALVE_SPRITE_NAME[13] = "sprite/valve";
    const char VALVE_BACKGROUND_NAME[18] = "bg/valve_minigame";

    uint32 lastStylusPos[2] = {0, 0};

    int rotation = 0;
    const int TARGET_ROTATION = 2048;

public:
    ValveMinigame();

    void Load();
    void Delete();
    void Update(volatile int frame, uint32 keys, Sound *sound);
    bool IsComplete();
    MinigameResult GetResult(int framesTaken);
    bool IsForCurrentBatch(int currentBatchProgress);
};