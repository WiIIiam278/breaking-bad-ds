#pragma once

struct Mineral
{
    const char name[32];
    const char description[256];
    const bool isSecret;
};

const int MINERAL_COUNT = 12;

enum MineralType
{
    MIENRAL_ANALCIME,
    MINERAL_TOPAZ,
    MINERAL_YTTRIUM,
    MINERAL_FLUORITE,
    MINERAL_AMBER,
    MINERAL_AQUAMARINE,
    MINERAL_RUBY,
    MINERAL_ALGODONITE,
    MINERAL_DIAMOND,
    MINERAL_CERIUM,
    MINERAL_QUARTZ,
    MINERAL_EMERALD
};

const Mineral MINERALS[MINERAL_COUNT] =
{
    {
        "Analcime",
        "Complete the tutorial",
        false
    },
    {
        "Topaz",
        "Cook a batch at 100\% purity",
        false
    },
    {
        "Yttrium",
        "Play as Jesse, yo",
        false
    },
    {
        "Fluorite",
        "Cook a batch at <40\% purity",
        false
    },
    {
        "Amber",
        "Slack off during the tutorial",
        false
    },
    {
        "Aquamarine",
        "Win a game of Multiplayer Vs.",
        false
    },
    {
        "Ruby",
        "Play as Kusuri, yep yep",
        true
    },
    {
        "Algodonite",
        "Hide from Gus' gaze for >30s",
        false
    },
    {
        "Diamond",
        "Perform a perfect quota clear",
        true
    },
    {
        "Cerium",
        "Buy explosives from Saul",
        true
    },
    {
        "Quartz",
        "Experience a Game Over",
        false
    },
    {
        "Emerald",
        "Complete Story Mode",
        false
    },
};