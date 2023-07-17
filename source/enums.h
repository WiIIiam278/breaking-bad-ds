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
    TITLE_SCREEN,
    DIALOGUE,
    MOVE,
    MINIGAME
};

enum Speaker
{
    GALE  
};