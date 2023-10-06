#include "minigames.h"

PourMinigame::PourMinigame()
{
}

void PourMinigame::Load()
{
    LoadBackground(POUR_BACKGROUND_NAME);
    showingTray = false;
    selectedSwitch = -1;
    currentPumpRate = 0;
    currentDrainRate = 0;
    targetPumpRate = 0;
    targetDrainRate = 0;

    NF_LoadSpriteGfx(POUR_SPRITES_NAME, POUR_SPRITES, 64, SPRITE_HEIGHT);
    NF_LoadSpritePal(POUR_SPRITES_NAME, POUR_SPRITES);
    NF_VramSpriteGfx(1, POUR_SPRITES, 0, false);
    NF_VramSpritePal(1, POUR_SPRITES, 0);

    UpdateTray();
    UpdateSwitches(0, nullptr);

    NF_CreateSprite(1, PIPE_SPRITE, 0, 0, PIPE_SPRITE_POS[0], PIPE_SPRITE_POS[1]);
    for (int i = 0; i < POURING_FLUID_COUNT; i++)
    {
        NF_CreateSprite(1, POURING_FLUID_SPRITES[i], 0, 0, PIPE_SPRITE_POS[0], PIPE_SPRITE_POS[1] + (32 + (32 * i)));
        NF_SpriteFrame(1, POURING_FLUID_SPRITES[i], 3);
        NF_ShowSprite(1, POURING_FLUID_SPRITES[i], false);
    }
}

void PourMinigame::Unload(Map *map)
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
    if (showingSwitches)
    {
        NF_DeleteSprite(1, PUMP_SWITCH_SPRITE);
        NF_DeleteSprite(1, DRAIN_SWITCH_SPRITE);
    }

    NF_DeleteSprite(1, PIPE_SPRITE);
    for (int i = 0; i < POURING_FLUID_COUNT; i++)
    {
        NF_DeleteSprite(1, POURING_FLUID_SPRITES[i]);
    }

    NF_UnloadSpriteGfx(POUR_SPRITES);
    NF_UnloadSpritePal(POUR_SPRITES);
    NF_FreeSpriteGfx(1, 0);
}

void PourMinigame::UpdateTray()
{
    const int BASE_TRAY_Y_POS = 160;
    for (int i = 0; i < TRAY_SPRITE_COUNT; i++)
    {
        int xCoord = ((SCREEN_WIDTH - (64 * 3)) / 2) + (64 * i);
        if (!showingTray)
        {
            NF_CreateSprite(1, TRAY_SPRITES[i], 0, 0, xCoord, BASE_TRAY_Y_POS);
            NF_CreateSprite(1, FLUID_SPRITES[i], 0, 0, xCoord, BASE_TRAY_Y_POS);
        }
        NF_SpriteFrame(1, TRAY_SPRITES[i], 7);
        NF_SpriteFrame(1, FLUID_SPRITES[i], 6);

        NF_MoveSprite(1, FLUID_SPRITES[i], xCoord, BASE_TRAY_Y_POS - ((int)(currentLevel * 2)));
    }
    showingTray = true;
}

void PourMinigame::UpdateSwitches(volatile int frame, Sound *sound)
{
    if (!showingSwitches)
    {
        NF_CreateSprite(1, PUMP_SWITCH_SPRITE, 0, 0, PUMP_SWITCH_POS[0], PUMP_SWITCH_POS[1]);
        NF_CreateSprite(1, DRAIN_SWITCH_SPRITE, 0, 0, DRAIN_SWITCH_POS[0], DRAIN_SWITCH_POS[1]);
    }

    if (keysHeld() & KEY_TOUCH)
    {
        touchPosition touch;
        touchRead(&touch);
        if (touch.px >= PUMP_SWITCH_POS[0] + 8 && touch.px <= PUMP_SWITCH_POS[0] + 56 &&
            touch.py >= PUMP_SWITCH_POS[1] - 8 && touch.py <= PUMP_SWITCH_POS[1] + 40)
        {
            if (selectedSwitch != 0)
            {
                sound->PlaySFX(SFX_CLICK);
            }
            selectedSwitch = 0;
            targetDrainRate = 0;
            targetPumpRate = MAX_PUMP_RATE;
        }
        else if (touch.px >= DRAIN_SWITCH_POS[0] + 8 && touch.px <= DRAIN_SWITCH_POS[0] + 56 &&
                 touch.py >= DRAIN_SWITCH_POS[1] - 8 && touch.py <= DRAIN_SWITCH_POS[1] + 40)
        {
            if (selectedSwitch != 1)
            {
                sound->PlaySFX(SFX_CLICK);
            }
            selectedSwitch = 1;
            targetDrainRate = MAX_DRAIN_RATE;
            targetPumpRate = 0;
        }
        else
        {
            if (selectedSwitch != -1)
            {
                sound->PlaySFX(SFX_CLICK);
            }
            selectedSwitch = -1;
            targetDrainRate = 0;
            targetPumpRate = 0;
        }
    }

    if (frame % 10 == 0)
    {
        currentLevel += currentPumpRate - currentDrainRate;
        currentPumpRate += (targetPumpRate - currentPumpRate) / 10.0f;
        currentDrainRate += (targetDrainRate - currentDrainRate) / 10.0f;

        if (currentPumpRate > MAX_PUMP_RATE)
        {
            currentPumpRate = MAX_PUMP_RATE;
        }
        else if (currentPumpRate < 0)
        {
            currentPumpRate = 0;
        }

        if (currentDrainRate > MAX_DRAIN_RATE)
        {
            currentDrainRate = MAX_DRAIN_RATE;
        }
        else if (currentDrainRate < 0)
        {
            currentDrainRate = 0;
        }

        if (currentLevel <= 0.1f)
        {
            currentLevel = 0;
        }
        else if (currentLevel >= MAX_LEVEL)
        {
            currentLevel = MAX_LEVEL;
        }
    }

    NF_SpriteFrame(1, PUMP_SWITCH_SPRITE, selectedSwitch == 0 ? 1 : 2);
    NF_SpriteFrame(1, DRAIN_SWITCH_SPRITE, selectedSwitch == 1 ? 1 : 2);

    showingSwitches = true;
}

void PourMinigame::Update(volatile int frame, uint32 keys, Sound *sound, Map *map)
{
    bool complete = IsComplete();
    for (int i = 0; i < POURING_FLUID_COUNT; i++)
    {
        NF_ShowSprite(1, POURING_FLUID_SPRITES[i], currentPumpRate > 0.1f && !complete);
        NF_SpriteFrame(1, POURING_FLUID_SPRITES[i], (currentPumpRate > (MAX_PUMP_RATE / 3.0f)) ? ((currentPumpRate >= MAX_PUMP_RATE / 1.5f) ? 5 : 4) : 3);
    }

    if (complete)
    {
        return;
    }

    UpdateTray();
    UpdateSwitches(frame, sound);
}

bool PourMinigame::IsComplete()
{
    if (currentDrainRate > 0.3f || currentPumpRate > 0.3f)
    {
        return false;
    }
    return currentLevel > TARGET_LEVEL - 0.5f && currentLevel < TARGET_LEVEL + 0.5f;
}

MinigameResult PourMinigame::GetResult(int framesTaken)
{
    return (framesTaken < 900) ? RESULT_GOOD : (framesTaken < 1450) ? RESULT_OKAY
                                                                    : RESULT_BAD;
}