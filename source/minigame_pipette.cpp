#include "minigames.h"

PipetteMinigame::PipetteMinigame()
{
}

void PipetteMinigame::Load()
{
}

void PipetteMinigame::Unload()
{
}

void PipetteMinigame::Update(volatile int frame, uint32 keys, Sound* sound)
{
    if (IsComplete())
    {
        return;
    }
}

bool PipetteMinigame::IsComplete()
{
    return true;
}

MinigameResult PipetteMinigame::GetResult(int framesTaken)
{
    return GOOD;
}

bool PipetteMinigame::IsForCurrentBatch(int currentBatchProgress)
{
    return currentBatchProgress == 3;
}