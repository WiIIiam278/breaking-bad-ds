#pragma once

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <memory>

#include <filesystem.h>
#include <nds.h>
#include <fat.h>

#include "minerals.h"

const int SAVE_FILE_VERSION = 1;

struct SaveFile
{
    int version;
    bool minerals[MINERAL_COUNT];
};

void SaveGame(SaveFile *saveFile, const char* fileName);
void LoadGame(SaveFile *saveFile, const char* fileName);
SaveFile *CreateNewSaveFile();