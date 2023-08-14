#pragma once

struct Mineral
{
    const char name[32];
    const char description[256];
    const int spriteFrame;
};

const int MINERAL_COUNT = 12;

enum MineralType
{
    MIENRAL_CORUNDUM,
};

const Mineral MINERALS[MINERAL_COUNT] =
{
    {
        "Corundum",
        "Completed the tutorial.",
        1
    }
};