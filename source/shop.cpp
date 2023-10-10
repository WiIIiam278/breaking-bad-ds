#include "shop.h"

Shop::Shop()
{
}

void Shop::Load(volatile int frame, Sound *sound)
{
    cursorPosition = -1;
    NF_LoadTiledBg(SAUL_BG, SAUL_BG, 256, 256);
    NF_CreateTiledBg(1, SAUL_BG_ID, SAUL_BG);

    sound->PlayBGM(BGM_SAUL, true);
    Transition(true, 30, TS_BOTTOM, frame);
}

// Returns true when the user backs out of the menu
bool Shop::Update(volatile int frame, Sound *sound)
{
    NF_ClearTextLayer(1, 0);
    if (keysDown() & KEY_B)
    {
        sound->PlaySFX(SFX_MENU_DRUM);
        return true;
    }

    if (keysDown() & KEY_UP)
    {
        cursorPosition--;

        if (cursorPosition < 0)
        {
            cursorPosition = POWER_UP_COUNT - 1;
        }
        sound->PlaySFX(SFX_MENU_SELECT);
    }
    else if (keysDown() & KEY_DOWN)
    {
        cursorPosition++;
        if (cursorPosition >= POWER_UP_COUNT)
        {
            cursorPosition = 0;
        }
        sound->PlaySFX(SFX_MENU_SELECT);
    }
    else if (keysDown() & KEY_A)
    {
        if (cursorPosition > -1 && globalSave.currentMoney >= POWER_UPS[cursorPosition].price)
        {
            sound->PlaySFX(SFX_SUCCESS_BELL);
            globalSave.powerUps[cursorPosition] = true;
            globalSave.currentMoney -= POWER_UPS[cursorPosition].price;
        }
        else
        {
            sound->PlaySFX(SFX_MENU_DRUM);
        }
    }

    if (cursorPosition == -1)
    {
        NF_WriteText(1, 0, 1, 1, "Ho-hoo! Look who we have here!");
        NF_WriteText(1, 0, 1, 3, "I've got some great deals.");
    }
    else
    {
        const PowerUp *selected = &POWER_UPS[cursorPosition];
        if (!globalSave.powerUps[cursorPosition])
        {
            char itemTopLine[48];
            sprintf(itemTopLine, "%s ($%i)", selected->name, selected->price);
            NF_WriteText(1, 0, 1, 1, itemTopLine);
            NF_WriteText(1, 0, 1, 3, selected->description);
        }
        else
        {
            NF_WriteText(1, 0, 1, 1, "SOLD OUT");
            NF_WriteText(1, 0, 1, 3, "Pleasure doin' business!");
        }
    }

    for (int i = 0; i < POWER_UP_COUNT; i++)
    {
        const PowerUp *item = &POWER_UPS[i];
        bool hasBought = globalSave.powerUps[i];

        char itemLine[32];
        if (cursorPosition == i)
        {
            sprintf(itemLine, "> %s", hasBought ? "SOLD OUT" : item->name);
        }
        else
        {
            sprintf(itemLine, "  %s", hasBought ? "SOLD OUT" : item->name);
        }
        NF_WriteText(1, 0, 16, 8 + (i * 2), itemLine);
    }

    char money[32];
    sprintf(money, "$%i", globalSave.currentMoney);
    NF_WriteText(1, 0, 24, 20, money);

    return false;
}

void Shop::Unload(volatile int frame, Sound *sound)
{
    Transition(false, 0, TS_BOTTOM, frame);
    NF_ClearTextLayer(1, 0);
    sound->StopBGM();
    
    NF_DeleteTiledBg(1, SAUL_BG_ID);
    NF_UnloadTiledBg(SAUL_BG);
}