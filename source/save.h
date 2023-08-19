#pragma once

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <memory>

#include <filesystem.h>
#include <nds.h>
#include <fat.h>

#include "enums.h"
#include "minerals.h"

const char SAVE_FILE_HEADER[4] = {'M', 'E', 'T', 'H'};
const uint16 SAVE_FILE_VERSION = 1;

struct SaveFile
{
    char fileHeader[4];
    uint16 version;

    uint16 storyModeDay;
    int storyModeMoney;
    bool storyModePowerUps[POWER_UP_COUNT];

    bool minerals[MINERAL_COUNT];
};

void SaveGame(SaveFile *saveFile, const char* fileName);
void LoadGame(SaveFile *saveFile, const char* fileName);
SaveFile *CreateNewSaveFile();