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

    pipePos[0] = -191;
    pipePos[1] = -78;
    lastStylusPos[0] = -1;
    lastStylusPos[1] = -1;
    rotatingCrank = -1;
    pipeInPlaceFrames = 0;
    pipeInPlace = false;

    u16 currentPos[2] = {pipePos[0], pipePos[1]};
    int specialPipe = (HORIZONTAL_PIPE_COUNT / 2);
    for (int i = 0; i < PIPE_COUNT; i++)
    {
        bool sideways = i < HORIZONTAL_PIPE_COUNT;
        NF_CreateSprite(1, PIPE_SPRITES[i], 0, 0, currentPos[0], currentPos[1]);
        NF_EnableSpriteRotScale(1, PIPE_SPRITES[i], PIPE_SPRITES[i], true);
        NF_SpriteRotScale(1, PIPE_SPRITES[i], (sideways && i != specialPipe) ? (-512 / 4) : 0, 120, 120);
        NF_SpriteFrame(1, PIPE_SPRITES[i], i == specialPipe ? 3 : (i == HORIZONTAL_PIPE_COUNT ? 1 : (i == PIPE_COUNT - 1 ? 2 : 0)));
        currentPos[0] += sideways ? PIPE_OFFSET : 0;
        currentPos[1] += sideways ? 0 : PIPE_OFFSET;
    }

    for (int i = 0; i < 2; i++)
    {
        crankRot[i] = 0;
        NF_CreateSprite(1, CRANK_SPRITES[i], 0, 0, CRANK_POS[i][0], CRANK_POS[i][1]);
        NF_SpriteFrame(1, CRANK_SPRITES[i], 4);
        NF_EnableSpriteRotScale(1, CRANK_SPRITES[i], CRANK_SPRITES[i], true);
        NF_SpriteRotScale(1, CRANK_SPRITES[i], crankRot[i], 256, 256);
    }
}

void MixMinigame::Unload()
{
    DeleteBackground(MIX_BACKGROUND_NAME);

    for (int i = 0; i < PIPE_COUNT; i++)
    {
        NF_DeleteSprite(1, PIPE_SPRITES[i]);
    }

    for (int i = 0; i < 2; i++)
    {
        NF_DeleteSprite(1, CRANK_SPRITES[i]);
    }

    NF_UnloadSpriteGfx(PIPE_SPRITES[0]);
    NF_UnloadSpritePal(PIPE_SPRITES[0]);
    NF_FreeSpriteGfx(1, 0);
}

void MixMinigame::Update(volatile int frame, uint32 keys, Sound *sound)
{
    if (IsComplete())
    {
        return;
    }

    if (pipeInPlace)
    {
        setRumble(pipeInPlaceFrames % 2);
        if (pipeInPlaceFrames == 45)
        {
            sound->PlaySFX(SFX_VACCUM);
        }
        pipeInPlaceFrames++;
        return;
    }

    rotatingCrank = -1;
    if (keys & KEY_TOUCH)
    {
        touchPosition touch;
        touchRead(&touch);
        u16 stylusPos[2] = {touch.px, touch.py};
        for (int i = 0; i < 2; i++)
        {
            int crankCenter[2] = {CRANK_POS[i][0] + 32, CRANK_POS[i][1] + 64};
            if (sqrt(pow(stylusPos[0] - crankCenter[0], 2) + pow(stylusPos[1] - crankCenter[1], 2)) < 64)
            {
                rotatingCrank = i;
                break;
            }
        }

        if (rotatingCrank != -1)
        {
            int scrubSpeed = abs((stylusPos[0] - lastStylusPos[0]) + (stylusPos[1] - lastStylusPos[1]));
            if ((lastStylusPos[0] != -1 && lastStylusPos[1] != -1) && scrubSpeed > 5)
            {
                crankRot[rotatingCrank] += crankRot[rotatingCrank] < 511 ? (crankRot[rotatingCrank] < 255 ? 2 * (scrubSpeed / 6) : (scrubSpeed / 4)) : 0;
            }

            if (frame % 3 == 0)
            {
                int otherCrank = rotatingCrank == 0 ? 1 : 0;
                crankRot[otherCrank] -= crankRot[otherCrank] > 0 ? (crankRot[otherCrank] < 255 ? 1 : 2) : 0;
                setRumble(scrubSpeed % 2);
            }

            if (scrubSpeed > 5 && (frame % 30 == 0))
            {
                sound->PlaySFX(SFX_CRANK);
            }
        }
        lastStylusPos[0] = stylusPos[0];
        lastStylusPos[1] = stylusPos[1];
    }
    else
    {
        lastStylusPos[0] = -1;
        lastStylusPos[1] = -1;

        if (frame % 3 == 0)
        {
            for (int i = 0; i < 2; i++)
            {
                crankRot[i] -= crankRot[i] > 0 ? (crankRot[i] < 255 ? 1 : 2) : 0;
            }
        }
    }

    u16 currentPos[2] = {pipePos[0] + (crankRot[0] / 4), pipePos[1] + (crankRot[1] / 4)};
    for (int i = 0; i < PIPE_COUNT; i++)
    {
        bool sideways = i < HORIZONTAL_PIPE_COUNT;
        if ((i % 2) != (frame % 4))
        {
            NF_MoveSprite(1, PIPE_SPRITES[i], currentPos[0], currentPos[1]);
        }
        currentPos[0] += sideways ? PIPE_OFFSET : 0;
        currentPos[1] += sideways ? 0 : PIPE_OFFSET;
    }

    for (int i = 0; i < 2; i++)
    {
        NF_SpriteRotScale(1, CRANK_SPRITES[i], crankRot[i], 256, 256);
        NF_SpriteFrame(1, CRANK_SPRITES[i], rotatingCrank == i ? 5 : 4);
    }

    if (currentPos[0] > TARGET_PIPE_POS[0] - TARGET_POS_VARIENCE && currentPos[0] < TARGET_PIPE_POS[0] + TARGET_POS_VARIENCE && currentPos[1] > TARGET_PIPE_POS[1] - TARGET_POS_VARIENCE && currentPos[1] < TARGET_PIPE_POS[1] + TARGET_POS_VARIENCE)
    {
        pipeInPlace = true;
        sound->PlaySFX(SFX_CLICK);
        setRumble(frame % 2);
    }
}

bool MixMinigame::IsComplete()
{
    return pipeInPlace && pipeInPlaceFrames > 190;
}

MinigameResult MixMinigame::GetResult(int framesTaken)
{
    return (framesTaken < 725) ? RESULT_GOOD : (framesTaken < 1225) ? RESULT_OKAY
                                                             : RESULT_BAD;
}