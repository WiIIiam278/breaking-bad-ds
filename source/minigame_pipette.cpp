#include "minigames.h"

PipetteMinigame::PipetteMinigame()
{
}

void PipetteMinigame::Load()
{
    LoadBackground(PIPETTE_BACKGROUND_NAME);
    showingTubes = false;
    pipetteLoc[0] = pipetteLoc[1] = 5;
    lastStylusPos[0] = lastStylusPos[1] = -1;

    NF_LoadSpriteGfx(PIPETTE_SPRITES_NAME, PIPETTE_SPRITES, 64, SPRITE_HEIGHT);
    NF_LoadSpritePal(PIPETTE_SPRITES_NAME, PIPETTE_SPRITES);
    NF_VramSpriteGfx(1, PIPETTE_SPRITES, 0, false);
    NF_VramSpritePal(1, PIPETTE_SPRITES, 0);

    for (int i = 0; i < PIPETTE_SEGMENTS; i++)
    {
        NF_CreateSprite(1, PIPETTE_SPRITE_IDS[i], 0, 0, pipetteLoc[0], pipetteLoc[1] + (SPRITE_HEIGHT * i));
        NF_SpriteFrame(1, PIPETTE_SPRITE_IDS[i], 7 + i);
    }

    UpdateTubes(0);
}

void PipetteMinigame::Unload()
{
    DeleteBackground(PIPETTE_BACKGROUND_NAME);

    if (showingTubes)
    {
        for (int t = 0; t < TUBE_COUNT; t++)
        {
            for (int s = 0; s < TUBE_SEGMENTS; s++)
            {
                const u32 spriteId = (TUBE_SPRITE_OFFSET + (TUBE_SEGMENTS * t) + s);
                NF_DeleteSprite(1, spriteId);
            }
        }
    }

    for (int i = 0; i < PIPETTE_SEGMENTS; i++)
    {
        NF_DeleteSprite(1, PIPETTE_SPRITE_IDS[i]);
    }

    NF_UnloadSpriteGfx(PIPETTE_SPRITES);
    NF_UnloadSpritePal(PIPETTE_SPRITES);
    NF_FreeSpriteGfx(1, 0);
}

void PipetteMinigame::Update(volatile int frame, uint32 keys, Sound *sound)
{
    if (IsComplete())
    {
        return;
    }

    if (keysHeld() & KEY_TOUCH)
    {
        touchPosition touch;
        touchRead(&touch);
        if (lastStylusPos[0] == -1)
        {
            lastStylusPos[0] = touch.px;
            lastStylusPos[1] = touch.py;
        }
        else
        {
            pipetteLoc[0] += touch.px - lastStylusPos[0];
            pipetteLoc[1] += touch.py - lastStylusPos[1];
            lastStylusPos[0] = touch.px;
            lastStylusPos[1] = touch.py;
        }
    }
    else
    {
        lastStylusPos[0] = -1;
        lastStylusPos[1] = -1;
    }

    for (int i = 0; i < PIPETTE_SEGMENTS; i++)
    {
        NF_MoveSprite(1, PIPETTE_SPRITE_IDS[i], pipetteLoc[0], pipetteLoc[1] + (SPRITE_HEIGHT * i));
    }

    UpdateTubes(frame);
}

void PipetteMinigame::UpdateTubes(volatile int frame)
{
    int currentPos[2];
    for (int t = 0; t < TUBE_COUNT; t++)
    {
        currentPos[0] = TUBE_BASE_POS[0] + (TUBE_SPACING * t);
        for (int s = 0; s < TUBE_SEGMENTS; s++)
        {
            const u32 spriteId = (TUBE_SPRITE_OFFSET + (TUBE_SEGMENTS * t) + s);
            currentPos[1] = TUBE_BASE_POS[1] + (SPRITE_HEIGHT * (TUBE_SEGMENTS - s));
            if (!showingTubes)
            {
                tubeBubbleAnimationFrames[t] = 0;
                tubeProgress[t] = 0;
                NF_CreateSprite(1, spriteId, 0, 0, currentPos[0], currentPos[1]);
            }

            if (s == TUBE_SEGMENTS - 1)
            {
                NF_SpriteFrame(1, spriteId, 6);
            }
            else if (s == TUBE_SEGMENTS - 2)
            {
                NF_SpriteFrame(1, spriteId, tubeProgress[t] >= TUBE_THRESHOLD ? 0 : 1);
            }
            else if (s == 0)
            {
                NF_SpriteFrame(1, spriteId, 5);
            }
            else
            {
                NF_SpriteFrame(1, spriteId, 4 - ((tubeBubbleAnimationFrames[t] > 10) ? ((tubeBubbleAnimationFrames[t] > 20) ? 2 : 1) : 0));
                if (tubeBubbleAnimationFrames[t] > 30)
                {
                    tubeBubbleAnimationFrames[t] = 11;
                }
                else if (tubeProgress[t] >= TUBE_THRESHOLD)
                {
                    tubeBubbleAnimationFrames[t]++;
                }
            }
        }
    }
    showingTubes = true;
}

bool PipetteMinigame::IsComplete()
{
    bool complete = true;
    for (int t = 0; t < TUBE_COUNT; t++)
    {
        complete &= (tubeProgress[t] >= TUBE_THRESHOLD);
    }
    return complete;
}

MinigameResult PipetteMinigame::GetResult(int framesTaken)
{
    return GOOD;
}