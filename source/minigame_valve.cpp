#include "minigames.h"

// Ventilaiton valve turning minigame
ValveMinigame::ValveMinigame()
{
}

void ValveMinigame::Load()
{
    rotation = 0;
    LoadBackground(VALVE_BACKGROUND_NAME);

    NF_LoadSpriteGfx(VALVE_SPRITE_NAME, VALVE_SPRITE, 64, 64);
    NF_LoadSpritePal(VALVE_SPRITE_NAME, VALVE_SPRITE);
    NF_VramSpriteGfx(1, VALVE_SPRITE, 0, false);
    NF_VramSpritePal(1, VALVE_SPRITE, 0);
    NF_CreateSprite(1, VALVE_SPRITE, 0, 0, 60, 37);
    NF_EnableSpriteRotScale(1, VALVE_SPRITE, VALVE_SPRITE, true);
    NF_SpriteRotScale(1, VALVE_SPRITE, 0, 386, 386);
}

void ValveMinigame::Unload(Map* map)
{
    DeleteBackground(VALVE_BACKGROUND_NAME);

    NF_DeleteSprite(1, VALVE_SPRITE);
    NF_UnloadSpriteGfx(VALVE_SPRITE);
    NF_UnloadSpritePal(VALVE_SPRITE);
    NF_FreeSpriteGfx(1, 0);
}

void ValveMinigame::Update(volatile int frame, uint32 keys, Sound *sound, Map *map, SaveFile *saveFile)
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

        // Play SFX, rumble
        if (frame % 120 == 0)
        {
            sound->PlaySFX(SFX_VALVE_TURN);
        }
        setRumble(rotation % 3 == 0);
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
    return rotation >= TARGET_ROTATION;
}

MinigameResult ValveMinigame::GetResult(int framesTaken)
{
    return (framesTaken < 480) ? RESULT_GOOD : (framesTaken < 900) ? RESULT_OKAY
                                                            : RESULT_BAD;
}