#pragma once

#include "enums.h"

struct Button
{
    const int buttonSprite;
    const int dimensions[2];
    const int textSprite;
    const MenuSelection selection;
};

struct Layout
{
    const int buttonCount;
    const Button buttons[4];
    const int buttonCoords[4][2];
    const int titleSprite;
    const int titleCoords[2];
    bool allowCaretNavigation;
    const MenuSelection caretMap[3][2];
    const MenuState backState;
    const int backgroundId;
    char* backgroundName;
};

static Layout LAYOUTS[12] = {

    {0,
     {},
     {},
     -1,
     {0, 0},
     false,
     {
         {NONE, NONE},
         {NONE, NONE},
         {NONE, NONE},
     },
     MENU_TITLE,
     1,
     "bg/title"
     },

    {4,
     {
         {3, {90, 58}, 2, OPEN_GAME_MENU},
         {0, {90, 36}, 6, START_TUTORIAL},
         {1, {90, 26}, 5, BACK_TO_TITLE},
         {2, {90, 26}, 7, OPEN_EXTRAS_MENU},
     },
     {
         {17, 55},
         {112, 39},
         {65, 112},
         {112, 75},
     },
     1,
     {98, 15},
     true,
     {
         {OPEN_GAME_MENU, START_TUTORIAL},
         {OPEN_GAME_MENU, OPEN_EXTRAS_MENU},
         {BACK_TO_TITLE, BACK_TO_TITLE},
     },
     MENU_TITLE,
     1,
     "bg/title"
     },

    {4,
     {
         {3, {90, 58}, 0, START_1P_GAME},
         {2, {90, 26}, 9, OPEN_ROOM},
         {1, {90, 26}, 5, BACK_TO_MAIN_MENU},
         {2, {90, 26}, 10, SEARCH_FOR_ROOMS},
     },
     {
         {17, 63},
         {112, 31},
         {112, 97},
         {112, 62},
     },
     2,
     {98, 15},
     true,
     {
         {START_1P_GAME, OPEN_ROOM},
         {START_1P_GAME, SEARCH_FOR_ROOMS},
         {START_1P_GAME, BACK_TO_MAIN_MENU},
     },
     MENU_MAIN,
     1,
     "bg/title"
     },

    {4,
     {
         {3, {90, 58}, 3, OPEN_MINERALS_MENU},
         {3, {90, 58}, 4, OPEN_SOUND_TEST_MENU},
         {1, {90, 26}, 5, BACK_TO_MAIN_MENU},
         {2, {90, 26}, 8, TOGGLE_RUMBLE},
     },
     {
         {17, 49},
         {112, 49},
         {112, 97},
         {17, 97},
     },
     7,
     {98, 15},
     true,
     {
         {OPEN_MINERALS_MENU, OPEN_SOUND_TEST_MENU},
         {OPEN_MINERALS_MENU, OPEN_SOUND_TEST_MENU},
         {TOGGLE_RUMBLE, BACK_TO_MAIN_MENU},
     },
     MENU_MAIN,
     1,
     "bg/title"
     },

     {1,
     {
         {1, {90, 26}, 5, BACK_TO_GAME_MENU},
     },
     {
         {65, 122},
     },
     9,
     {98, 23},
     true,
     {
        {BACK_TO_GAME_MENU, NONE},
        {NONE, NONE},
        {NONE, NONE},
     },
     MENU_GAME_SELECT,
     2,
     "bg/guassian"
     },

    {1,
     {
         {1, {90, 26}, 5, BACK_TO_GAME_MENU},
     },
     {
         {65, 122},
     },
     10,
     {98, 23},
     true,
     {
        {BACK_TO_GAME_MENU, NONE},
        {NONE, NONE},
        {NONE, NONE},
     },
     MENU_GAME_SELECT,
     2,
     "bg/guassian"
     },

     {1,
     {
         {1, {90, 26}, 5, BACK_TO_EXTRAS_MENU},
     },
     {
         {65, 122},
     },
     4,
     {98, 23},
     true,
     {
        {BACK_TO_GAME_MENU, NONE},
        {NONE, NONE},
        {NONE, NONE},
     },
     MENU_EXTRAS,
     2,
     "bg/guassian"
     },

     {1,
     {
         {1, {90, 26}, 5, BACK_TO_EXTRAS_MENU},
     },
     {
         {65, 122},
     },
     8,
     {98, 23},
     true,
     {
        {BACK_TO_GAME_MENU, NONE},
        {NONE, NONE},
        {NONE, NONE},
     },
     MENU_EXTRAS,
     2,
     "bg/guassian"
     },

     {1,
     {
         {1, {90, 26}, 5, BACK_TO_EXTRAS_MENU},
     },
     {
         {4, SCREEN_HEIGHT - 54},
     },
     -1,
     {162, SCREEN_HEIGHT - 38},
     true,
     {
        {BACK_TO_GAME_MENU, NONE},
        {NONE, NONE},
        {NONE, NONE},
     },
     MENU_EXTRAS,
     3,
     "bg/minerals_hank"
     }

};

static Layout *GetLayoutForState(MenuState state)
{
    switch (state)
    {
    case MENU_TITLE:
        return &LAYOUTS[0];
    case MENU_MAIN:
        return &LAYOUTS[1];
    case MENU_GAME_SELECT:
        return &LAYOUTS[2];
    case MENU_EXTRAS:
        return &LAYOUTS[3];
    case MENU_MP_HOST_ROOM:
        return &LAYOUTS[4];
    case MENU_MP_JOIN_ROOM:
        return &LAYOUTS[5];
    case MENU_MUSIC_PLAYER:
        return &LAYOUTS[6];
    case MENU_RUMBLE:
        return &LAYOUTS[7];
    case MENU_MINERALS:
        return &LAYOUTS[8];
    default:
        return nullptr;
    }
};