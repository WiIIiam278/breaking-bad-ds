#include "minigames.h"

PourMinigame::PourMinigame()
{
}

void PourMinigame::Load()
{
}

void PourMinigame::Unload()
{
}

void PourMinigame::Update(volatile int frame, uint32 keys, Sound* sound)
{
    if (IsComplete())
    {
        return;
    }
}

bool PourMinigame::IsComplete()
{
    return true;
}

MinigameResult PourMinigame::GetResult(int framesTaken)
{
    return GOOD;
}