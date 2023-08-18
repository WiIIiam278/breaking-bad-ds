#include "minigames.h"

PipetteMinigame::PipetteMinigame()
{
}

void PipetteMinigame::Load()
{
    LoadBackground(PIPETTE_BACKGROUND_NAME);
    showingTubes = false;
    pipettePos[0] = pipettePos[1] = 5;
    lastStylusPos[0] = lastStylusPos[1] = -1;
    dropletsMissed = 0;
    showDroplet = true;

    NF_LoadSpriteGfx(PIPETTE_SPRITES_NAME, PIPETTE_SPRITES, 64, SPRITE_HEIGHT);
    NF_LoadSpritePal(PIPETTE_SPRITES_NAME, PIPETTE_SPRITES);
    NF_VramSpriteGfx(1, PIPETTE_SPRITES, 0, false);
    NF_VramSpritePal(1, PIPETTE_SPRITES, 0);

    for (int i = 0; i < PIPETTE_SEGMENTS; i++)
    {
        NF_CreateSprite(1, PIPETTE_SPRITE_IDS[i], 0, 0, pipettePos[0], pipettePos[1] + (SPRITE_HEIGHT * i));
        NF_SpriteFrame(1, PIPETTE_SPRITE_IDS[i], 7 + i);
    }

    NF_CreateSprite(1, DROPLET_SPRITE, 0, 0, pipettePos[0], pipettePos[1] + (SPRITE_HEIGHT * 2) + 4);
    NF_SpriteFrame(1, DROPLET_SPRITE, 10);

    UpdateTubes(0);
}

void PipetteMinigame::Unload(Map* map)
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
    NF_DeleteSprite(1, DROPLET_SPRITE);

    NF_UnloadSpriteGfx(PIPETTE_SPRITES);
    NF_UnloadSpritePal(PIPETTE_SPRITES);
    NF_FreeSpriteGfx(1, 0);
}

void PipetteMinigame::Update(volatile int frame, uint32 keys, Sound *sound, Map *map)
{
    UpdateTubes(frame);

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
            pipettePos[0] += touch.px - lastStylusPos[0];
            pipettePos[1] += touch.py - lastStylusPos[1];
            lastStylusPos[0] = touch.px;
            lastStylusPos[1] = touch.py;
        }
    }
    else
    {
        lastStylusPos[0] = -1;
        lastStylusPos[1] = -1;
    }

    pipettePos[0] = std::max(0, std::min(SCREEN_WIDTH - 64, pipettePos[0]));
    pipettePos[1] = std::max(-20, std::min(SCREEN_HEIGHT - 80 - (SPRITE_HEIGHT * PIPETTE_SEGMENTS), pipettePos[1]));

    for (int i = 0; i < PIPETTE_SEGMENTS; i++)
    {
        NF_MoveSprite(1, PIPETTE_SPRITE_IDS[i], pipettePos[0], pipettePos[1] + (SPRITE_HEIGHT * i));
    }

    if (!dropletDropping)
    {
        dropletPos[0] = pipettePos[0];
        dropletPos[1] = pipettePos[1] + (SPRITE_HEIGHT * 2) + 4;
        dropletSplashing = false;
        showDroplet = true;
        dropletYVel = 0;
        dropletFrames = 0;
    }

    if (keysUp() & KEY_TOUCH)
    {
        dropletDropping = true;
    }

    if (dropletDropping)
    {
        dropletPos[1] += dropletYVel;
        dropletYVel = std::min(10, dropletYVel + (frame % 4 == 0 ? 1 : 0));

        bool isDropletInTube = false;
        int dropTube = -1;
        const int TUBE_WIDTH = 48;
        for (int t = 0; t < TUBE_COUNT; t++)
        {
            const int tubeBuffer = ((64 - TUBE_WIDTH) / 2) + 5;
            const int tubeStart = TUBE_BASE_POS[0] + (TUBE_SPACING * t) - tubeBuffer;
            const int tubeEnd = tubeStart + TUBE_WIDTH - tubeBuffer;
            if (dropletPos[0] >= tubeStart && dropletPos[0] <= tubeEnd)
            {
                isDropletInTube = tubeProgress[t] < TUBE_THRESHOLD || dropletSplashing;
                dropTube = t;
                break;
            }
        }

        if (dropletPos[1] > (isDropletInTube ? DROPLET_TARGET_Y_CONTACT : DROPLET_MISS_Y_CONTACT))
        {
            dropletSplashing = true;
            dropletYVel = 0;
            dropletPos[1] = isDropletInTube ? DROPLET_TARGET_Y_CONTACT : DROPLET_MISS_Y_CONTACT;
            dropletFrames++;

            if (dropletFrames > 8)
            {
                dropletDropping = false;
                showDroplet = false;
                dropletPos[0] = pipettePos[0];
                dropletPos[1] = pipettePos[1] + (SPRITE_HEIGHT * 2);

                if (isDropletInTube)
                {
                    tubeProgress[dropTube] = TUBE_THRESHOLD;
                }
                else
                {
                    dropletsMissed++;
                }
            }
        }
    }

    NF_ShowSprite(1, DROPLET_SPRITE, showDroplet);
    NF_MoveSprite(1, DROPLET_SPRITE, dropletPos[0], dropletPos[1]);
    NF_SpriteFrame(1, DROPLET_SPRITE, dropletSplashing ? 11 : 10);
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
    if (dropletsMissed > 2)
    {
        return RESULT_BAD;
    }
    else if (dropletsMissed > 0)
    {
        return RESULT_OKAY;
    }
    return (framesTaken < 650) ? RESULT_GOOD : (framesTaken < 1000) ? RESULT_OKAY
                                                                    : RESULT_BAD;
}