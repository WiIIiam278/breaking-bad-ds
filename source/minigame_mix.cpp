#include "minigames.h"

MixMinigame::MixMinigame()
{
}

void MixMinigame::Load()
{
    LoadBackground(MIX_BACKGROUND_NAME);

    NF_LoadSpriteGfx(PIPE_SPRITES_NAME, PIPE_SPRITES[0], 64, 64);
    NF_LoadSpritePal(PIPE_SPRITES_NAME, PIPE_SPRITES[0]);
    NF_VramSpriteGfx(1, PIPE_SPRITES[0], 0, false);
    NF_VramSpritePal(1, PIPE_SPRITES[0], 0);

    u16 currentPos[2] = { PIPE_ORIGIN_POS[0], PIPE_ORIGIN_POS[1] };
    for (int i = 0; i < PIPE_COUNT; i++)
    {
        bool sideways = i < 8;
        NF_CreateSprite(1, PIPE_SPRITES[i], 0, 0, currentPos[0], currentPos[1]);
        NF_EnableSpriteRotScale(1, PIPE_SPRITES[i], PIPE_SPRITES[i], true);
        NF_SpriteRotScale(1, PIPE_SPRITES[i], (sideways && i != 4) ? (-512 / 4) : 0, 120, 120);
        NF_SpriteFrame(1, PIPE_SPRITES[i], i == 4 ? 3 : (i == 8 ? 1 : (i == PIPE_COUNT - 1 ? 2 : 0)));
        currentPos[0] += sideways ? PIPE_OFFSET : 0;
        currentPos[1] += sideways ? 0 : PIPE_OFFSET;
    }
}

void MixMinigame::Unload()
{
    DeleteBackground(MIX_BACKGROUND_NAME);

    for (int i = 0; i < PIPE_COUNT; i++)
    {
        NF_DeleteSprite(1, PIPE_SPRITES[i]);
    }
    
    NF_UnloadSpriteGfx(PIPE_SPRITES[0]);
    NF_UnloadSpritePal(PIPE_SPRITES[0]);
    NF_FreeSpriteGfx(1, 0);
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
    return false;
}

MinigameResult MixMinigame::GetResult(int framesTaken)
{
    return GOOD;
}