#include "minigames.h"

// General minigame logic
void LoadBackground(const char name[])
{
    NF_LoadTiledBg(name, name, 256, 256);
    NF_CreateTiledBg(1, 2, name);
}

void DeleteBackground(const char name[])
{
    NF_UnloadTiledBg(name);
    NF_DeleteTiledBg(1, 2);
}


// Ventilaiton valve turning minigame
ValveMinigame::ValveMinigame()
{
}

void ValveMinigame::Load()
{
    LoadBackground(VALVE_BACKGROUND_NAME);

    NF_LoadSpriteGfx(VALVE_SPRITE_NAME, VALVE_SPRITE, 64, 64);
    NF_LoadSpritePal(VALVE_SPRITE_NAME, VALVE_SPRITE);
    NF_VramSpriteGfx(1, VALVE_SPRITE, 0, false);
    NF_VramSpritePal(1, VALVE_SPRITE, 0);
    NF_CreateSprite(1, VALVE_SPRITE, 0, 0, 60, 37);
    NF_EnableSpriteRotScale(1, VALVE_SPRITE, VALVE_SPRITE, true);
    NF_SpriteRotScale(1, VALVE_SPRITE, 0, 386, 386);
}

void ValveMinigame::Delete()
{
    DeleteBackground(VALVE_BACKGROUND_NAME);

    NF_DeleteSprite(1, VALVE_SPRITE);
    NF_UnloadSpriteGfx(VALVE_SPRITE);
    NF_UnloadSpritePal(VALVE_SPRITE);
    NF_FreeSpriteGfx(1, 0);
}

void ValveMinigame::Update(volatile int frame, uint32 keys)
{
    if (IsComplete())
    {
        return;
    }

    // Check if the user is rotating the valve
    if (keys & KEY_TOUCH)
    {
        touchPosition touch;
        touchRead(&touch);

        // Calculate the angle between the last touch position and the current touch position
        int angle = atan2(touch.px - lastStylusPos[0], touch.py - lastStylusPos[1]) * 180 / M_PI;

        // Update the rotation of the valve with speed in mind
        rotation += angle / VALVE_ROTATION_SPEED;

        // Update the last touch position
        lastStylusPos[0] = touch.px;
        lastStylusPos[1] = touch.py;

        // Rotate the valve
        NF_SpriteRotScale(1, VALVE_SPRITE, rotation, 386, 386);
    }
    else
    {
        // Reset the last touch position
        lastStylusPos[0] = 0;
        lastStylusPos[1] = 0;
    }
}

bool ValveMinigame::IsComplete()
{
    return rotation >= targetRotation;
}

MinigameResult ValveMinigame::GetResult(int framesTaken)
{
    return (framesTaken < 480) ? GOOD : (framesTaken < 900) ? OKAY : BAD;
}

bool ValveMinigame::IsForCurrentBatch(int currentBatchProgress)
{
    return currentBatchProgress == 0;
}