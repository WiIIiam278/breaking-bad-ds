#pragma once

enum Direction
{
    LEFT,
    DOWN,
    RIGHT,
    UP
};

enum Tile
{
    // Fallback tile (not walkable)
    VOID,
    // Collision tiles (not walkable)
    WALL,
    // Empty floor tiles 
    FLOOR,
    // Valve minigame
    MINIGAME_VALVE
};

enum Mode
{
    MAIN_MENU,
    DIALOGUE,
    MOVE,
    MINIGAME
};

enum Speaker
{
    GALE  
};

enum MenuSelection
{
    NONE,
    START_GAME,
    START_TUTORIAL,
    BACK_TO_TITLE,
    TOGGLE_RUMBLE
};

enum MenuState
{
    LOGO,
    TITLE,
    MENU,
    RUMBLE,
    LOADING
};

enum MinigameResult
{
    GOOD,
    OKAY,
    BAD
};