#include "minigames.h"

MixMinigame::MixMinigame()
{
}

void MixMinigame::Load()
{
}

void MixMinigame::Unload()
{
}

void MixMinigame::Update(volatile int frame, uint32 keys, Sound* sound)
{
    if (IsComplete())
    {
        return;
    }
}

bool MixMinigame::IsComplete()
{
    return true;
}

MinigameResult MixMinigame::GetResult(int framesTaken)
{
    return GOOD;
}

bool MixMinigame::IsForCurrentBatch(int currentBatchProgress)
{
    return currentBatchProgress == 2;
}