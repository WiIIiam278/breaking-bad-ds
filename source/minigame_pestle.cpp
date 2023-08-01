#include "minigames.h"

PestleMinigame::PestleMinigame()
{
}

void PestleMinigame::Load()
{
}

void PestleMinigame::Unload()
{
}

void PestleMinigame::Update(volatile int frame, uint32 keys, Sound* sound)
{
    if (IsComplete())
    {
        return;
    }
}

bool PestleMinigame::IsComplete()
{
    return false;
}

MinigameResult PestleMinigame::GetResult(int framesTaken)
{
    return GOOD;
}

bool PestleMinigame::IsForCurrentBatch(int currentBatchProgress)
{
    return currentBatchProgress == 1;
}