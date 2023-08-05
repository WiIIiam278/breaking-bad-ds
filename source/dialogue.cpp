#include "dialogue.h"

Dialogue::Dialogue()
{
}

void Dialogue::Load(ScriptId script, volatile int frame)
{
    Start(SCRIPTS[script].speaker, SCRIPTS[script].lines, SCRIPTS[script].emotes, SCRIPTS[script].length, frame);
}

void Dialogue::Start(Speaker speaker, const char script[][128], const Emote emotes[64], int scriptLength, int startFrame)
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
        currentEmotes[i] = emotes[i];
    }

    // Set lab background
    NF_LoadTiledBg(PORTRAITS[currentSpeaker].bgName, PORTRAITS[currentSpeaker].bgName, 256, 256);
    NF_CreateTiledBg(1, SCRIPT_BG, PORTRAITS[currentSpeaker].bgName);

    // Load sprites
    NF_LoadSpriteGfx(PORTRAITS[currentSpeaker].spriteName, 1, 64, 64);
    NF_LoadSpritePal(PORTRAITS[currentSpeaker].spriteName, 1);
    NF_VramSpriteGfx(1, 1, 0, false);
    NF_VramSpritePal(1, 1, 0);

    // Load emotes
    NF_LoadSpriteGfx(DIALOGUE_EMOTES_SPRITE_NAME, 2, 32, 32);
    NF_LoadSpritePal(DIALOGUE_EMOTES_SPRITE_NAME, 2);
    NF_VramSpriteGfx(1, 2, 2, false);
    NF_VramSpritePal(1, 2, 2);
    NF_CreateSprite(1, DIALOGUE_EMOTES_SPRITE, 2, 2, DIALOGUE_EMOTE_POS[0], DIALOGUE_EMOTE_POS[1]);
    NF_ShowSprite(1, DIALOGUE_EMOTES_SPRITE, false);
    NF_EnableSpriteRotScale(1, DIALOGUE_EMOTES_SPRITE, 12, true);
    NF_SpriteRotScale(1, DIALOGUE_EMOTES_SPRITE, 0, 512, 512);

    // Create, position & scale sprite
    NF_CreateSprite(1, 10, 0, 0, 64, -21);
    NF_SpriteFrame(1, 10, PORTRAITS[currentSpeaker].eyes[0]);
    NF_CreateSprite(1, 11, 0, 0, 64, 43);
    NF_SpriteFrame(1, 11, PORTRAITS[currentSpeaker].mouthes[0]);
    NF_EnableSpriteRotScale(1, 10, 12, true);
    NF_EnableSpriteRotScale(1, 11, 12, true);
    NF_SpriteRotScale(1, 10, 0, 512, 512);
    NF_SpriteRotScale(1, 11, 0, 512, 512);
}

bool Dialogue::Update(volatile int frame, uint32 keys, Sound *sound)
{
    Emote lineEmote = currentEmotes[currentLineIndex];
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

    // Update emotes
    if (lineEmote != EMOTE_NONE)
    {
        if (lineEmote == EMOTE_EXCLAMATION && charsToPrint < 4)
        {
            setRumble(frame % 2);
        }
        NF_SpriteFrame(1, DIALOGUE_EMOTES_SPRITE, lineEmote);
        NF_ShowSprite(1, DIALOGUE_EMOTES_SPRITE, true);

        // Emote bob animation
        int bob = ((frame % 100) > 50 ? (100 - (frame % 100)) : (frame % 100)) / (10 - (lineEmote / 2));
        NF_MoveSprite(1, DIALOGUE_EMOTES_SPRITE, DIALOGUE_EMOTE_POS[0], DIALOGUE_EMOTE_POS[1] + bob);
    }
    else
    {
        NF_ShowSprite(1, DIALOGUE_EMOTES_SPRITE, false);
    }

    // Blink animation
    int blinkRange = (lastBlink - frame);
    bool isBlinking = false;
    if (blinkRange < 10 && blinkRange > 0)
    {
        isBlinking = true;
    }
    if (blinkRange <= 0)
    {
        lastBlink = frame + (rand() % 100) + 100;
    }
    NF_SpriteFrame(1, 10, PORTRAITS[currentSpeaker].eyes[isBlinking]);

    // Update mouth animation
    if (!endOfLine)
    {
        int speechFrame = (frame - currentLineStartFrame);
        if (speechFrame % 5 == 0)
        {
            sound->PlaySFX(SFX_DIALOGUE_BLEEP);
        }
        if (speechFrame % 10 == 0)
        {
            currentSpeakerAnimation++;
            if (currentSpeakerAnimation > 3)
            {
                currentSpeakerAnimation = 0;
            }
            NF_SpriteFrame(1, 11, PORTRAITS[currentSpeaker].mouthes[currentSpeakerAnimation]);
        }
    }
    else
    {
        NF_SpriteFrame(1, 11, PORTRAITS[currentSpeaker].mouthes[0]);
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
            sound->PlaySFX(SFX_MENU_SELECT);

            if (currentLineIndex >= currentScriptLength)
            {
                return true;
            }
        }
    }
    else if (keys & KEY_START)
    {
        currentLineStartFrame = -(lineLength * 10);
        charsToPrint = lineLength;
        endOfLine = true;
        return true;
    }

    char lineToPrint[charsToPrint];
    strncpy(lineToPrint, currentLine, charsToPrint);
    NF_ClearTextLayer(1, 0);

    // Print dialogue lines
    int line1Chars = charsToPrint > CHARACTERS_PER_DIALOGUE_LINE ? CHARACTERS_PER_DIALOGUE_LINE : charsToPrint;
    char line1[line1Chars];
    strncpy(line1, lineToPrint, line1Chars);
    line1[line1Chars] = '\0';
    NF_WriteText(1, 0, 1, 18, line1);

    int line2Chars = charsToPrint > CHARACTERS_PER_DIALOGUE_LINE ? charsToPrint - CHARACTERS_PER_DIALOGUE_LINE : 0;
    if (line2Chars > 0)
    {
        char line2[line2Chars];
        strncpy(line2, lineToPrint + CHARACTERS_PER_DIALOGUE_LINE, line2Chars);
        line2[line2Chars] = '\0';
        NF_WriteText(1, 0, 1, 20, line2);
    }
    if (endOfLine)
    {
        indicatorFrame++;
        NF_WriteText(1, 0, 30, 20, (indicatorFrame > 30 ? ">" : " "));
        if (indicatorFrame > 60)
        {
            indicatorFrame = 0;
        }
    }
    return false;
}

void Dialogue::Unload()
{
    // Remove bg
    NF_DeleteTiledBg(1, SCRIPT_BG);
    NF_UnloadTiledBg(PORTRAITS[currentSpeaker].bgName);

    // Remove sprites
    NF_DeleteSprite(1, 10);
    NF_DeleteSprite(1, 11);
    NF_ShowSprite(1, DIALOGUE_EMOTES_SPRITE, false);
    NF_DeleteSprite(1, DIALOGUE_EMOTES_SPRITE);
    NF_ClearTextLayer(1, 0);

    // Remove stuff from RAM, VRAM
    NF_UnloadSpriteGfx(1);
    NF_UnloadSpritePal(1);
    NF_FreeSpriteGfx(1, 0);
    NF_UnloadSpriteGfx(2);
    NF_UnloadSpritePal(2);
    NF_FreeSpriteGfx(1, 2);
}

int Dialogue::GetTutorialDialogue(int tutorialProgress, int batchProgress, Tile playerTile)
{
    switch (tutorialProgress)
    {
        case 0:
            return SCRIPT_GALE_TUTORIAL_INTRO;
        case 1:
            return playerTile == MINIGAME_VALVE ? SCRIPT_GALE_TUTORIAL_VALVE : -1;
        case 2:
            return batchProgress == 1 ? SCRIPT_GALE_TUTORIAL_VALVE_COMPLETE : -1;
    }
    return -1;
}