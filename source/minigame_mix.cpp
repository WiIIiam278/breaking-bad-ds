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
    rotatingCrank = -1;

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

    u16 currentPos[2] = {pipePos[0] + (crankRot[0] / 4), pipePos[1] + (crankRot[1] / 4)};
    for (int i = 0; i < PIPE_COUNT; i++)
    {
        bool sideways = i < HORIZONTAL_PIPE_COUNT;
        NF_MoveSprite(1, PIPE_SPRITES[i], currentPos[0], currentPos[1]);
        currentPos[0] += sideways ? PIPE_OFFSET : 0;
        currentPos[1] += sideways ? 0 : PIPE_OFFSET;
    }

    // Rotate cranks
    for (int i = 0; i < 2; i++)
    {
        // Allow the user to touch the crank to rotate it. The crank should rotate to the touch position.
        int crankCenter[2] = {CRANK_POS[i][0] + 32, CRANK_POS[i][1] + 64};
        if ((keys & KEY_TOUCH) && (rotatingCrank == i || rotatingCrank == -1))
        {
            touchPosition touch;
            touchRead(&touch);
            u16 stylusPos[2] = {touch.px, touch.py};
            
            // if the user touched within a radius of 64 pixels of the crank center, rotate the crank
            if (sqrt(pow(stylusPos[0] - crankCenter[0], 2) + pow(stylusPos[1] - crankCenter[1], 2)) < 64)
            {
                rotatingCrank = i;                    
                crankRot[i] += crankRot[i] < 511 ? (crankRot[i] < 255 ? 2 : 1) : 0;
            }

            lastStylusPos[0] = stylusPos[0];
            lastStylusPos[1] = stylusPos[1];
        }
        else
        {
            rotatingCrank = -1;
            if ((crankRot[i] > 0) && (frame % 3 == 0))
            {
                crankRot[i] -= (crankRot[i] < 255 ? 1 : 2);
            }
            lastStylusPos[0] = 0;
            lastStylusPos[1] = 0;
        }

        NF_SpriteRotScale(1, CRANK_SPRITES[i], crankRot[i], 256, 256);
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