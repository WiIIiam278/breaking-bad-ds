#pragma once

struct Level
{
    int quota;
    int time;
};

const int LEVEL_COUNT = 5;
const Level LEVEL_STORY_MODE[LEVEL_COUNT + 1] = {
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
        1 /* 14 */,
        215
    },
    {
        1 /* 16 */,
        235
    },
    {
        1,
        60
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