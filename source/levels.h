#pragma once

struct Level
{
    int quota;
    int time;
};

const int LEVEL_COUNT = 7;
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
        200,
    },
    {
        14,
        200
    },
    {
        15,
        180
    },
    {
        15,
        170
    },
    {
        17,
        170
    }
};

const Level LEVEL_TUTORIAL = {
    5,
    -1
};

const Level LEVEL_MULTIPLAYER_VS = {
    99,
    180
};