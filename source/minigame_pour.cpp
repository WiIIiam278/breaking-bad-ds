#include "minigames.h"

PourMinigame::PourMinigame()
{
}

void PourMinigame::Load()
{
    LoadBackground(POUR_BACKGROUND_NAME);
    showingTray = false;

    NF_LoadSpriteGfx(POUR_SPRITES_NAME, POUR_SPRITES, 64, SPRITE_HEIGHT);
    NF_LoadSpritePal(POUR_SPRITES_NAME, POUR_SPRITES);
    NF_VramSpriteGfx(1, POUR_SPRITES, 0, false);
    NF_VramSpritePal(1, POUR_SPRITES, 0);

    UpdateTray();
}

void PourMinigame::Unload()
{
    DeleteBackground(POUR_BACKGROUND_NAME);

    if (showingTray)
    {
        for (int i = 0; i < TRAY_SPRITE_COUNT; i++)
        {
            NF_DeleteSprite(1, TRAY_SPRITES[i]);
            NF_DeleteSprite(1, FLUID_SPRITES[i]);
        }
    }

    NF_UnloadSpriteGfx(POUR_SPRITES);
    NF_UnloadSpritePal(POUR_SPRITES);
    NF_FreeSpriteGfx(1, 0);
}

void PourMinigame::UpdateTray()
{
    for (int i = 0; i < TRAY_SPRITE_COUNT; i++)
    {
        if (!showingTray)
        {
            int xCoord = ((SCREEN_WIDTH - (64 * 3)) / 2) + (64 * i);
            NF_CreateSprite(1, TRAY_SPRITES[i], 0, 0, xCoord, 160);
            NF_CreateSprite(1, FLUID_SPRITES[i], 0, 0, xCoord, 160);
        }
        NF_SpriteFrame(1, TRAY_SPRITES[i], 7);
        NF_SpriteFrame(1, FLUID_SPRITES[i], 6);
    }
    showingTray = true;
}

void PourMinigame::Update(volatile int frame, uint32 keys, Sound *sound)
{
    if (IsComplete())
    {
        return;
    }
    
    UpdateTray();
}

bool PourMinigame::IsComplete()
{
    return false;
}

MinigameResult PourMinigame::GetResult(int framesTaken)
{
    return RESULT_GOOD;
}