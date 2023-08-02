#include "minigames.h"

CrackMinigame::CrackMinigame()
{
}

void CrackMinigame::Load()
{
}

void CrackMinigame::Unload()
{
}

void CrackMinigame::Update(volatile int frame, uint32 keys, Sound* sound)
{
    if (IsComplete())
    {
        return;
    }
}

bool CrackMinigame::IsComplete()
{
    return true;
}

MinigameResult CrackMinigame::GetResult(int framesTaken)
{
    return GOOD;
}