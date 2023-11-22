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

    UpdateSprites(0, nullptr, false);
}

void CrackMinigame::Unload(Map* map)
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

void CrackMinigame::UpdateSprites(volatile int frame, Sound *sound, bool steelHammer)
{
    for (int x = 0; x < SPRITE_COUNTS[0]; x++)
    {
        for (int y = 0; y < SPRITE_COUNTS[1]; y++)
        {
            int spriteId = CRACK_SPRITE_BASE_ID + (x * SPRITE_COUNTS[1]) + y;
            int pos[2] = {CRACK_SPRITE_POS[0] + (x * SPRITE_DIMS[0]), CRACK_SPRITE_POS[1] + (y * SPRITE_DIMS[1])};
            if (!showingSprites && !IsComplete())
            {
                NF_CreateSprite(1, spriteId, 0, 0, pos[0], pos[1]);
                NF_HflipSprite(1, spriteId, rand() % 2 == 0);
                spriteDamageTable[x][y] = 0;
                spritePositions[x][y][0] = pos[0];
                spritePositions[x][y][1] = pos[1];
            }

            int grid[2] = {x, y};
            if (ProcessSprite(frame, spriteId, grid, sound, steelHammer))
            {
                showingSprites = true;
                return;
            }
        }
    }
    showingSprites = true;
}

bool CrackMinigame::ProcessSprite(volatile int frame, int spriteId, int grid[2], Sound *sound, bool steelHammer)
{
    int x = spritePositions[grid[0]][grid[1]][0];
    int y = spritePositions[grid[0]][grid[1]][1];
    NF_MoveSprite(1, spriteId, x, y);

    u32 damage = spriteDamageTable[grid[0]][grid[1]];
    bool bagged = damage == -1;
    if (bagged)
    {
        if (y < SCREEN_HEIGHT)
        {
            spritePositions[grid[0]][grid[1]][1] += 3;
            return false;
        }
        NF_ShowSprite(1, spriteId, false);
        return false;
    }

    if (IsComplete())
    {
        return false;
    }

    touchPosition touch;

    if (damage < 5)
    {
        NF_SpriteFrame(1, spriteId, 7 + damage);

        if (keysDown() & KEY_TOUCH)
        {
            touchRead(&touch);

            if (touch.px >= x && touch.px <= x + SPRITE_DIMS[0] && touch.py >= y && touch.py <= y + SPRITE_DIMS[1])
            {
                bool criticalHit = frame % 2 == 0 || steelHammer;
                spriteDamageTable[grid[0]][grid[1]] += criticalHit ? 3 : (frame % 3 == 0) ? 1 : 2;
                if (spriteDamageTable[grid[0]][grid[1]] >= 5)
                {
                    NF_SpriteFrame(1, spriteId, rand() % 7);
                    spriteDamageTable[grid[0]][grid[1]] = 6;
                }
                
                if (criticalHit) {
                    sound->PlaySFX(SFX_PESTLE);
                }

                rumble(criticalHit);
                return true;
            }
        }
        return false;
    }

    if (keysHeld() & KEY_TOUCH)
    {
        touchRead(&touch);

        if (touch.px >= x && touch.px <= x + SPRITE_DIMS[0] && touch.py >= y && touch.py <= y + SPRITE_DIMS[1])
        {
            spritePositions[grid[0]][grid[1]][0] = touch.px - (SPRITE_DIMS[0] / 2);
            spritePositions[grid[0]][grid[1]][1] = touch.py - (SPRITE_DIMS[1] / 2);

            if (y > SCREEN_HEIGHT - 64)
            {
                spriteDamageTable[grid[0]][grid[1]] = -1;
            }
            return true;
        }
    }
    return false;
}

void CrackMinigame::Update(volatile int frame, uint32 keys, Sound *sound, Map *map)
{
    UpdateSprites(frame, sound, globalSave.powerUps[PWR_STEEL_HAMMER]);
}

bool CrackMinigame::IsComplete()
{
    bool complete = true;
    for (int x = 0; x < SPRITE_COUNTS[0]; x++)
    {
        for (int y = 0; y < SPRITE_COUNTS[1]; y++)
        {
            complete &= spriteDamageTable[x][y] == -1;
        }
    }
    return complete;
}

MinigameResult CrackMinigame::GetResult(int framesTaken)
{
    return (framesTaken < 1025) ? RESULT_GOOD : (framesTaken < 1425) ? RESULT_OKAY
                                                                     : RESULT_BAD;
}