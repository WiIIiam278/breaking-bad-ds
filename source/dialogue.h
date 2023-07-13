#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <string>

#include "enums.h"

class Dialogue
{
private:
    Speaker speaker;
    std::string text;
    
    int startFrame;
    const int CHAR_DELAY = 3;
    
    int GetCharsToDraw(volatile int frame);

public:
    Dialogue(Speaker speaker, int frame, std::string text[5]);

    std::string GetPortraitPath();
    void Draw(volatile int frame);
};