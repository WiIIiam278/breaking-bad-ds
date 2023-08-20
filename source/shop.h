#pragma once

#include "enums.h"
#include "sound.h"
#include "save.h"
#include "transitions.h"

#include <nds.h>
#include <nf_lib.h>

struct PowerUp
{
    const char name[32];
    const char description[128];
    const int price;
};

const PowerUp POWER_UPS[5] = 
{
    {
        "Air Jordans",
        "Increases movement speed",
        659,
    },
    {
        "Steel Hammer",
        "Increases meth cracking speed",
        789
    },
    {
        "Golden Pestle",
        "Increases pestling speed",
        549
    },
    {
        "Watch Battery",
        "Gain a time bonus",
        1299
    },
    {
        "Explosives",
        "(...)",
        2999
    }
};

class Shop
{
private:
    const char SAUL_BG[32] = "bg/saul";
    const u32 SAUL_BG_ID = 1;

    int cursorPosition = -1;

public:
    Shop();

    void Load(volatile int frame, Sound *sound, SaveFile *saveFile);
    bool Update(volatile int frame, Sound *sound, SaveFile *saveFile);
    void Unload(volatile int frame, Sound *sound);
};