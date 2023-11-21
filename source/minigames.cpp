#include "minigames.h"

// General minigame logic
void LoadBackground(const char name[])
{
    NF_LoadTiledBg(name, name, 256, 256);
    NF_CreateTiledBg(1, 2, name);
}

void DeleteBackground(const char name[])
{
    NF_UnloadTiledBg(name);
    NF_DeleteTiledBg(1, 2);
}