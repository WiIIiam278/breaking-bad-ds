#include "minigames.h"

CrackMinigame::CrackMinigame()
{
}

void CrackMinigame::Load()
{
    LoadBackground(CRACK_BACKGROUND_NAME);

    NF_LoadSpriteGfx(CRACK_SPRITES_NAME, CRACK_SPRITE_BASE_ID, SPRITE_DIMS[0], SPRITE_DIMS[1]);
    NF_LoadSpritePal(CRACK_SPRITES_NAME, CRACK_SPRITE_BASE_ID);
    NF_VramSpriteGfx(1, CRACK_SPRITE_BASE_ID, 0, false);
    NF_VramSpritePal(1, CRACK_SPRITE_BASE_ID, 0);

    UpdateSprites(0);
}

void CrackMinigame::Unload()
{
    DeleteBackground(CRACK_BACKGROUND_NAME);

    for (int x = 0; x < SPRITE_COUNTS[0]; x++)
    {
        for (int y = 0; y < SPRITE_COUNTS[1]; y++)
        {
            int spriteId = CRACK_SPRITE_BASE_ID + (x * SPRITE_COUNTS[1]) + y;
            NF_DeleteSprite(1, spriteId);
        }
    }

    NF_UnloadSpriteGfx(CRACK_SPRITE_BASE_ID);
    NF_UnloadSpritePal(CRACK_SPRITE_BASE_ID);
    NF_FreeSpriteGfx(1, 0);
}

void CrackMinigame::UpdateSprites(volatile int frame)
{
    for (int x = 0; x < SPRITE_COUNTS[0]; x++)
    {
        for (int y = 0; y < SPRITE_COUNTS[1]; y++)
        {
            int spriteId = CRACK_SPRITE_BASE_ID + (x * SPRITE_COUNTS[1]) + y;
            int pos[2] = { CRACK_SPRITE_POS[0] + (x * SPRITE_DIMS[0]), CRACK_SPRITE_POS[1] + (y * SPRITE_DIMS[1]) };
            if (!showingSprites)
            {
                NF_CreateSprite(1, spriteId, 0, 0, pos[0], pos[1]);
            }
            NF_SpriteFrame(1, spriteId, 7);
        }
    }
    showingSprites = true;
}

void CrackMinigame::Update(volatile int frame, uint32 keys, Sound* sound)
{
    if (IsComplete())
    {
        return;
    }

    UpdateSprites(frame);
}

bool CrackMinigame::IsComplete()
{
    return false;
}

MinigameResult CrackMinigame::GetResult(int framesTaken)
{
    return RESULT_GOOD;
}