#include "dialogue.h"

Dialogue::Dialogue()
{
}

void Dialogue::Load(ScriptId script, volatile int frame)
{
    Start(SCRIPTS[script].speaker, SCRIPTS[script].lines, SCRIPTS[script].length, frame);
}

void Dialogue::Start(Speaker speaker, const char script[][128], int scriptLength, int startFrame)
{
    // Set dialog params
    currentSpeaker = speaker;
    currentLineIndex = 0;
    currentLineStartFrame = startFrame;
    currentSpeakerAnimation = 0;
    currentScriptLength = scriptLength;

    // copy script to currentScript
    for (int i = 0; i < scriptLength; i++)
    {
        strncpy(currentScript[i], script[i], 128);
    }

    // Set lab background
    NF_LoadTiledBg(LAB_BG_NAME, LAB_BG_NAME, 256, 256);
    NF_CreateTiledBg(1, LAB_BG, LAB_BG_NAME);

    // Load sprite files from NitroFS
    switch (currentSpeaker)
    {
    case GALE:
        NF_LoadSpriteGfx("sprite/gale", 1, 64, 64);
        NF_LoadSpritePal("sprite/gale", 1);
        break;
    }
    NF_VramSpriteGfx(1, 1, 0, false);
    NF_VramSpritePal(1, 1, 0);

    // Create, position & scale sprite
    NF_CreateSprite(1, currentSpeaker + 10, 0, 0, 64, 32);
    NF_EnableSpriteRotScale(1, currentSpeaker + 10, currentSpeaker + 10, true);
    NF_SpriteRotScale(1, currentSpeaker + 10, 0, 386, 386);
}

bool Dialogue::Update(volatile int frame, uint32 keys, Sound *sound)
{
    int charsToPrint = (frame - currentLineStartFrame) / 3;
    char currentLine[128];
    strncpy(currentLine, currentScript[currentLineIndex], 128);
    bool endOfLine = false;
    int lineLength = strlen(currentLine);
    if (charsToPrint >= lineLength)
    {
        endOfLine = true;
        charsToPrint = lineLength;
    }

    // Update speaker animation
    if (!endOfLine)
    {
        int speechFrame = (frame - currentLineStartFrame);
        if (speechFrame % 5 == 0)
        {
            sound->PlaySFX(DIALOGUE_BLEEP);
        }
        if (speechFrame % 10 == 0)
        {
            currentSpeakerAnimation++;
            if (currentSpeakerAnimation > 7)
            {
                currentSpeakerAnimation = 0;
            }
            NF_SpriteFrame(1, currentSpeaker + 10, currentSpeakerAnimation);
        }
    }

    // Handle input
    if (((keys & KEY_A) || (keys & KEY_TOUCH)))
    {
        if (!endOfLine)
        {
            currentLineStartFrame = -(lineLength * 10);
            charsToPrint = lineLength;
            endOfLine = true;
        }
        else
        {
            currentLineIndex++;
            currentLineStartFrame = frame;
            sound->PlaySFX(MENU_SELECT);

            if (currentLineIndex >= currentScriptLength)
            {
                return true;
            }
        }
    }

    char lineToPrint[charsToPrint];
    strncpy(lineToPrint, currentLine, charsToPrint);
    NF_ClearTextLayer(1, 0);

    // Print dialogue lines
    int line1Chars = charsToPrint > CHARACTERS_PER_DIALOGUE_LINE ? CHARACTERS_PER_DIALOGUE_LINE : charsToPrint;
    char line1[line1Chars];
    strncpy(line1, lineToPrint, line1Chars);
    line1[line1Chars] = '\0';
    NF_WriteText(1, 0, 1, 21, line1);

    int line2Chars = charsToPrint > CHARACTERS_PER_DIALOGUE_LINE ? charsToPrint - CHARACTERS_PER_DIALOGUE_LINE : 0;
    if (line2Chars > 0)
    {
        char line2[line2Chars];
        strncpy(line2, lineToPrint + CHARACTERS_PER_DIALOGUE_LINE, line2Chars);
        line2[line2Chars] = '\0';
        NF_WriteText(1, 0, 1, 22, line2);
    }
    return false;
}

void Dialogue::Unload()
{
    NF_DeleteTiledBg(1, LAB_BG);
    NF_UnloadTiledBg(LAB_BG_NAME);
    NF_DeleteSprite(1, currentSpeaker + 10);
    NF_ClearTextLayer(1, 0);

    // Unload from RAM, VRAM
    NF_UnloadSpriteGfx(1);
    NF_UnloadSpritePal(1);
    NF_FreeSpriteGfx(1, 0);
}

int Dialogue::GetTutorialDialogue(int tutorialProgress)
{
    switch (tutorialProgress)
    {
        case 0:
            return SCRIPT_GALE_TUTORIAL_INTRO;
        case 1:
            return SCRIPT_GALE_TUTORIAL_VALVE;
        case 2:
            return SCRIPT_GALE_TUTORIAL_VALVE_COMPLETE;
    }
    return -1;
}