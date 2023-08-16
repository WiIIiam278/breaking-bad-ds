#pragma once

struct Level
{
    int quota;
    int time;
};

const int LEVEL_COUNT = 7;
const Level LEVEL_STORY_MODE[LEVEL_COUNT] = {
    {
        10,
        240
    },
    {
        12,
        240
    },
    {
        14,
        200,
    },
    {
        16,
        200
    },
    {
        18,
        180
    },
    {
        20,
        180
    },
    {
        25,
        160
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