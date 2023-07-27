#pragma once

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <memory>

#include <filesystem.h>
#include <nds.h>
#include <NEMain.h>
#include <nf_lib.h>

#include "enums.h"
#include "sound.h"

struct Script
{
    Speaker speaker;
    int length;
    const char lines[64][128];
};

enum ScriptId
{
    SCRIPT_GALE_TUTORIAL_INTRO,
    SCRIPT_GALE_TUTORIAL_VALVE,
    SCRIPT_GALE_TUTORIAL_VALVE_COMPLETE,
    SCRIPT_GALE_TUTORIAL_IDLE,
};

const Script SCRIPTS[64] = {
    {GALE,
     6,
     {"GALE: M-Mister White!",
      "GALE: How good it is to see  you again!",
      "GALE: I-I'd be delighted to  give you a little refresher.",
      "GALE: Allow me to take the   lead now!",
      "GALE: First and foremost,    head to the control valve.",
      "GALE: I've marked it on the  floor plan with a fun red X!"}},
    {GALE,
     5,
     {
         "GALE: Very good! This valve  controls the lab ventilation.",
         "GALE: By the way, did you    see the Batch Checklist?",
         "GALE: As you work, items     will be checked off.",
         "GALE: You'll need to cook    batches to finish your quota!",
         "GALE: Now! Let's turn on     the lab ventillation!",
     }},
    {GALE,
     9,
     {
         "GALE: Heh, looks like that   valve needs some oil.",
         "GALE: The purity of the      batch depends on our speed!",
         "GALE: Don't forget: Dilly-   dally for too long at a task",
         "GALE: and the quality of     the cook will suffer!",
         "GALE: This will affect the   yield. In other words...",
         "GALE: How much the batch     will contribute to the quota.",
         "GALE: Be sure to check the   Purity Meter to mind this.",
         "GALE: Next up, we'll need    to pestle the critical solutes",
         "GALE: to a fine powder for   dissolving. To the mortar!",
     }},
    {GALE,
     5,
     {
         "GALE: M-Mister White, we're  wasting time!",
         "GALE: We need to get back    to work!",
         "GALE: Are you feeling okay?",
         "GALE: If you're lost, check  the Floor Plan.",
         "GALE: Your next step is      marked with a little red X!",
     }}
};

class Dialogue
{
    private:
        const int CHARACTERS_PER_DIALOGUE_LINE = 29;
        const int LAB_BG = 3;
        const char LAB_BG_NAME[32] = "bg/lab";

        Speaker currentSpeaker = GALE;
        char currentScript[64][128];
        int currentScriptLength = 0;
        int currentLineIndex = 0;
        volatile int currentLineStartFrame = 0;
        int currentSpeakerAnimation = 0;

        void Start(Speaker speaker, const char script[][128], int scriptLength, int startFrame);
    public:
        Dialogue();
        void Load(ScriptId script, volatile int frame);
        bool Update(volatile int frame, uint32 keys, Sound *sound);
        void Unload();
        int GetTutorialDialogue(int tutorialProgress);
};