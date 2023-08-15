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
    OPEN_EXTRAS_MENU,
    OPEN_SOUND_TEST_MENU,
    OPEN_MINERALS_MENU,
    BACK_TO_EXTRAS_MENU
};

enum MenuState
{
    // Title group
    MENU_LOGO,
    MENU_TITLE,
    
    // Main menu group
    MENU_MAIN,
    MENU_LOADING,

    // Game selection group
    MENU_GAME_SELECT,
    MENU_MP_HOST_ROOM,
    MENU_MP_JOIN_ROOM,

    // Extras group
    MENU_EXTRAS,
    MENU_RUMBLE,
    MENU_MUSIC_PLAYER,
    MENU_MINERALS,
};

enum MinigameResult
{
    RESULT_GOOD,
    RESULT_OKAY,
    RESULT_BAD
};

enum TransitionScreen
{
    TS_TOP = 0,
    TS_BOTTOM = 1,
    TS_BOTH = 2
};

enum Character
{
    CHAR_WALT = 0,
    CHAR_JESSIE = 1,
    CHAR_YEPPERS = 2
};