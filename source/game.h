#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <string>

#include <filesystem.h>
#include <nds.h>
#include <NEMain.h>
#include <nf_lib.h>

#include "map.h"
#include "player.h"
#include "enums.h"

class Game
{
private:
    Mode mode;

    Map map;
    Player player;
    NE_Camera *camera;
    
    volatile int frame;

public:
    Game();

    void WaitLoop();
    void Prepare3DGraphics();
    void Prepare2DGraphics();
    void Update(volatile int frame);
    void Render(volatile int frame);
};