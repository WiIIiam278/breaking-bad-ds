#pragma once

enum Mode
{
    MAIN_MENU,
    DIALOGUE,
    MOVE,
    MINIGAME,
    GAME_OVER,
    PAUSED
};

enum Direction
{
    LEFT,
    DOWN,
    RIGHT,
    UP
};

enum GameType
{
    TUTORIAL_GAME,
    SINGLEPLAYER_GAME,
    MULTIPLAYER_GAME
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
    MINIGAME_MIX,
    MINIGAME_PIPETTE,
    MINIGAME_POUR,
    MINIGAME_CRACK
};

enum Speaker
{
    GALE
};

enum MenuSelection
{
    NONE,
    OPEN_GAME_MENU,
    START_TUTORIAL,
    BACK_TO_TITLE,
    TOGGLE_RUMBLE,
    START_1P_GAME,
    OPEN_ROOM,
    BACK_TO_MAIN_MENU,
    SEARCH_FOR_ROOMS,
    SKIP_LOGO,
    START_MP_GAME,
    BACK_TO_GAME_MENU,
};

enum MenuState
{
    // Title group
    MENU_LOGO,
    MENU_TITLE,
    
    // Main menu group
    MENU_MAIN,
    MENU_RUMBLE,
    MENU_LOADING,
    MENU_SOUND_TEST,

    // Game selection group
    MENU_GAME_SELECT,
    MENU_MP_HOST_ROOM,
    MENU_MP_JOIN_ROOM
};

enum MinigameResult
{
    GOOD,
    OKAY,
    BAD
};