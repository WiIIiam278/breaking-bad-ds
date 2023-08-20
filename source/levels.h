#pragma once

struct Level
{
    int quota;
    int time;
};

const int LEVEL_COUNT = 5;
const Level LEVEL_STORY_MODE[LEVEL_COUNT] = {
    {
        8,
        240
    },
    {
        10,
        225
    },
    {
        12,
        225,
    },
    {
        14,
        215
    },
    {
        16,
        235
    }
};

const Level LEVEL_TUTORIAL = {
    5,
    -1
};

const Level LEVEL_MULTIPLAYER_VS = {
    99,
    300
};