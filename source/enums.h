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
    // Minigames
    MINIGAME_VALVE,
    MINIGAME_PESTLE,
    MINIGAME_MIXING,
    MINIGAME_PIPPETTE,
    MINIGAME_TRAYS,
    MINIGAME_PROVE
};

enum Mode
{
    MAIN_MENU,
    DIALOGUE,
    MOVE,
    MINIGAME,
    GAME_OVER,
    PAUSED
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
    TOGGLE_RUMBLE,
    SKIP_LOGO
};

enum MenuState
{
    LOGO,
    TITLE,
    MENU,
    RUMBLE_INFO,
    LOADING,
    SOUND_TEST
};

enum MinigameResult
{
    GOOD,
    OKAY,
    BAD
};