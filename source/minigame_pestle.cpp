#include "minigames.h"

PestleMinigame::PestleMinigame()
{
}

void PestleMinigame::Load()
{
    LoadBackground(PESTLE_BACKGROUND_NAME);

    NF_LoadSpriteGfx(PESTLE_SPRITES_NAME, PESTLE_SPRITE, 64, 64);
    NF_LoadSpritePal(PESTLE_SPRITES_NAME, PESTLE_SPRITE);
    NF_VramSpriteGfx(1, PESTLE_SPRITE, 0, false);
    NF_VramSpritePal(1, PESTLE_SPRITE, 0);
    NF_CreateSprite(1, PESTLE_SPRITE, 0, 0, 60, 37);
    NF_EnableSpriteRotScale(1, PESTLE_SPRITE, PESTLE_SPRITE, true);
    NF_SpriteRotScale(1, PESTLE_SPRITE, 0, 300, 300);
    for (int i = 0; i < 3; i++)
    {
        solutePos[i][0] = 30 + (i * 35);
        solutePos[i][1] = i == 1 ? 67 : 59;
        soluteRot[i] = (i * 90);
        soluteDamage[i] = 0;
        NF_CreateSprite(1, SOLUTE_SPRITES[i], 0, 0, solutePos[i][0], solutePos[i][1]);
        NF_SpriteFrame(1, SOLUTE_SPRITES[i], 1);
        NF_EnableSpriteRotScale(1, SOLUTE_SPRITES[i], SOLUTE_SPRITES[i], true);
        NF_SpriteRotScale(1, SOLUTE_SPRITES[i], soluteRot[i], 120, 120);
    }
}

void PestleMinigame::Unload()
{
    DeleteBackground(PESTLE_BACKGROUND_NAME);

    NF_DeleteSprite(1, PESTLE_SPRITE);
    for (int i = 0; i < 3; i++)
    {
        NF_DeleteSprite(1, SOLUTE_SPRITES[i]);
    }
    NF_UnloadSpriteGfx(PESTLE_SPRITE);
    NF_UnloadSpritePal(PESTLE_SPRITE);
    NF_FreeSpriteGfx(1, 0);
}

void PestleMinigame::Update(volatile int frame, uint32 keys, Sound *sound)
{
    if (IsComplete())
    {
        return;
    }

    if (keys & KEY_TOUCH)
    {
        touchPosition touch;
        touchRead(&touch);

        pestlePos[0] = touch.px - 64;
        pestlePos[1] = std::min(43, touch.py - 64);
        if (pestlePos[1] == 43)
        {
            setRumble(frame % 2 == 0);
        }

        if (frame - lastVelocityCheckFrame > SWING_LENGTH)
        {
            lastVelocityCheckFrame = frame;
            velocity = (pestlePos[1] - lastPestlePos[1]) / 60;
            lastPestlePos[0] = pestlePos[0];
            lastPestlePos[1] = pestlePos[1];
        }
    }
    else
    {
        lastVelocityCheckFrame = frame;
    }

    NF_MoveSprite(1, PESTLE_SPRITE, pestlePos[0], pestlePos[1]);
    NF_SpriteRotScale(1, PESTLE_SPRITE, ((pestlePos[0] + 240) + 180 / 90), 300, 300);

    for (int i = 0; i < 3; i++)
    {
        if (soluteDamage[i] < SOLUTE_MAX_DAMAGE && abs(pestlePos[0] - solutePos[i][0]) < PESTLE_PROXIMITY && abs(pestlePos[1] - solutePos[i][1]) < PESTLE_PROXIMITY && velocity > VELOCITY_THRESHOLD)
        {
            soluteDamage[i]++;
            velocity -= 150;

            if (frame - lastSfxPlay > SFX_DURATION)
            {
                lastSfxPlay = frame;
                sound->PlaySFX(SFX_PESTLE);
            }

            solutePos[i][0] = std::max(std::min(solutePos[i][0] + (rand() % 10) - 5, 100), 30);
            soluteRot[i] += 512 / 24;
        }

        NF_SpriteFrame(1, SOLUTE_SPRITES[i], soluteDamage[i] > (SOLUTE_MAX_DAMAGE / 2) ? soluteDamage[i] >= SOLUTE_MAX_DAMAGE ? 3 : 2 : 1);
        NF_MoveSprite(1, SOLUTE_SPRITES[i], solutePos[i][0], solutePos[i][1]);
        NF_SpriteRotScale(1, SOLUTE_SPRITES[i], soluteRot[i], 120, 120);
    }
}

bool PestleMinigame::IsComplete()
{
    bool complete = true;
    for (int i = 0; i < 3; i++)
    {
        complete &= soluteDamage[i] >= SOLUTE_MAX_DAMAGE;
    }
    return complete;
}

MinigameResult PestleMinigame::GetResult(int framesTaken)
{
    return (framesTaken < 680) ? RESULT_GOOD : (framesTaken < 1150) ? RESULT_OKAY
                                                             : RESULT_BAD;
}