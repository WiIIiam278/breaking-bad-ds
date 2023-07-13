#include "dialogue.h"

Dialogue::Dialogue(Speaker speaker, int frame, std::string text[5])
{
    this->speaker = speaker;
    this->text = text[0];
    this->startFrame = frame;
}

std::string Dialogue::GetPortraitPath()
{
    switch (this->speaker)
    {
    case GALE:
        return "sprite/gale";
    }
}

int Dialogue::GetCharsToDraw(volatile int frame)
{
    int chars = (frame - this->startFrame) / this->CHAR_DELAY;
    return text.size() < chars ? text.size() : chars;
}

void Dialogue::Draw(volatile int frame)
{
    int charsToDraw = this->GetCharsToDraw(frame);
    std::string textToDraw = this->text.substr(0, charsToDraw);
    
}