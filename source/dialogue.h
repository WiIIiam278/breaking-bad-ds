#pragma once

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <memory>

#include <filesystem.h>
#include <nds.h>
#include <NEMain.h>
#include <nf_lib.h>

#include "sound.h"
#include "enums.h"

enum Speaker
{
    GALE,
    GUS
};

enum Emote
{
    EMOTE_NONE = -1,
    EMOTE_EXCLAMATION = 0,
    EMOTE_QUESTION = 1,
    EMOTE_ANGRY = 2
};

struct Script
{
    const Speaker speaker;
    const int length;
    const char lines[64][128];
    const Emote emotes[64];
};

struct Portrait
{
    const Speaker speaker;
    const char spriteName[32];
    const char bgName[32];
    const int eyes[2];
    const int mouthes[4];
};

enum ScriptId
{
    SCRIPT_GUS_TEST,
    SCRIPT_GALE_TUTORIAL_IDLE,
    SCRIPT_GALE_TUTORIAL_INTRO,
    SCRIPT_GALE_TUTORIAL_VALVE,
    SCRIPT_GALE_TUTORIAL_VALVE_COMPLETE,
};

const Script SCRIPTS[64] = {
    {GUS,
     5,
     {"Walter.",
      "We have a deal, Walter.",
      "You're going to cook for me.",
      "And you will do so with...      ...efficiency.",
      "300 pounds a week. That is your goal."},
     {EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE}},
    {GALE,
     5,
     {
         "M-Mister White, we're wastingtime!",
         "We need to get back to work!",
         "Are you feeling okay?",
         "If you're lost, check the    Floor Plan.",
         "Your next step is marked witha little red X!",
     },
     {EMOTE_EXCLAMATION,
      EMOTE_NONE,
      EMOTE_QUESTION,
      EMOTE_NONE,
      EMOTE_NONE}},
    {GALE,
     6,
     {"M-Mister White!",
      "How good it is to see you    again!",
      "I-I'd be delighted to give   you a little refresher.",
      "Allow me to take the lead!",
      "First and foremost, head to  the control valve.",
      "I've marked it on the floor  plan with a fun red X!"},
     {EMOTE_EXCLAMATION,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE}},
    {GALE,
     5,
     {
         "Very good! You've turned on  the lab ventilation.",
         "By the way, did you see the  Batch Checklist?",
         "As you work, items will be   checked off.",
         "You'll need to cook batches  to finish your quota!",
         "Now! Let's turn on the lab   ventillation!",
     },
     {EMOTE_NONE,
      EMOTE_QUESTION,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_EXCLAMATION}},
    {GALE,
     9,
     {
         "Heh, looks like that valve   needs some oil.",
         "The purity of the batch      depends on our speed!",
         "Don't forget: Dilly-dally    for too long at a task...",
         "and the quality of the cook  will suffer!",
         "This will affect the yield.  In other words...",
         "How much the batch will      contribute to the quota.",
         "Be sure to check the Purity  Meter to mind this.",
         "Next up, we'll need to       pestle the critical solutes",
         "...to a fine powder for      dissolving. To the mortar!",
     },
     {EMOTE_NONE,
      EMOTE_EXCLAMATION,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE}},
};

const Portrait PORTRAITS[64] = {
    {GALE, "sprite/gale", "bg/lab_gale", {0, 1}, {2, 3, 4, 5}},
    {GUS, "sprite/gus", "bg/pollos_gus", {0, 1}, {2, 3, 4, 5}},
};

class Dialogue
{
private:
    const char DIALOGUE_EMOTES_SPRITE_NAME[32] = "sprite/dialogue_emotes";
    const int DIALOGUE_EMOTE_POS[2] = {144, -10};
    const int CHARACTERS_PER_DIALOGUE_LINE = 29;
    const int SCRIPT_BG = 1;
    const int DIALOGUE_EMOTES_SPRITE = 9;

    Speaker currentSpeaker = GALE;
    char currentScript[64][128];
    Emote currentEmotes[64];
    int currentScriptLength = 0;
    int currentLineIndex = 0;
    volatile int currentLineStartFrame = 0;
    volatile int indicatorFrame = 0;
    int currentSpeakerAnimation = 0;
    int lastBlink = 0;

    void Start(Speaker speaker, const char script[][128], const Emote emotes[64], int scriptLength, int startFrame);

public:
    Dialogue();
    void Load(ScriptId script, volatile int frame);
    bool Update(volatile int frame, uint32 keys, Sound *sound);
    void Unload();
    int GetTutorialDialogue(int tutorialProgress, int batchProgress, Tile playerTile);
};