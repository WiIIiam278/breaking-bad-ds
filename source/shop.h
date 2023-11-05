#pragma once

#include <nds.h>
#include <nf_lib.h>

#include "enums.h"
#include "sound.h"
#include "transitions.h"
#include "save.hpp"

struct PowerUp
{
    const char name[32];
    const char description[128];
    const int price;
};

const PowerUp POWER_UPS[POWER_UP_COUNT] = 
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
        1949
    }
};

class Shop
{
private:
    const char SAUL_BG[32] = "bg/saul";
    const u32 SAUL_BG_ID = 1;

    int cursorPosition = -1;

    const int DIALOGUE_SPEED = 5;
    int dialogueStartFrame = 0;

public:
    Shop();

    void Load(volatile int frame, Sound *sound);
    bool Update(volatile int frame, Sound *sound);
    void WriteSaulDialogue(const char* topLine, const char* bottomLine, volatile int frame, Sound *sound);
    void Unload(volatile int frame, Sound *sound);
};