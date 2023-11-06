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
        265
    },
    {
        10,
        245
    },
    {
        12,
        240,
    },
    {
        14,
        230
    },
    {
        16,
        250
    },
    {
        1,
        60
    }
};

/* const Level LEVEL_STORY_MODE[LEVEL_COUNT + 1] = {
    { 1, 999 },
    { 1, 999 },
    { 1, 999 },
    { 1, 999 },
    { 1, 999 },
    { 1, 999 }
}; */


const Level LEVEL_TUTORIAL = {
    5,
    -1
};

const Level LEVEL_MULTIPLAYER_VS = {
    99,
    300
};