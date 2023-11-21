#pragma once

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <memory>

#include <filesystem.h>
#include <nds.h>
#include <NEMain.h>
#include <nf_lib.h>

#include "save.hpp"
#include "sound.h"
#include "enums.h"
#include "levels.h"
#include "rumble.h"

enum Speaker
{
  DUMMY,
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
  SCRIPT_DUMMY_TEST,
  SCRIPT_GUS_IDLE,
  SCRIPT_GALE_TUTORIAL_IDLE,
  SCRIPT_GALE_TUTORIAL_INTRO,
  SCRIPT_GALE_TUTORIAL_VALVE,
  SCRIPT_GALE_TUTORIAL_VALVE_COMPLETE,
  SCRIPT_GALE_TUTORIAL_PESTLE,
  SCRIPT_GALE_TUTORIAL_PESTLE_COMPLETE,
  SCRIPT_GALE_TUTORIAL_MIX,
  SCRIPT_GALE_TUTORIAL_MIX_COMPLETE,
  SCRIPT_GALE_TUTORIAL_PIPETTE,
  SCRIPT_GALE_TUTORIAL_PIPETTE_COMPLETE,
  SCRIPT_GALE_TUTORIAL_POUR,
  SCRIPT_GALE_TUTORIAL_POUR_COMPLETE,
  SCRIPT_GALE_TUTORIAL_CRACK,
  SCRIPT_GALE_TUTORIAL_CRACK_COMPLETE,
  SCRIPT_GUS_TUTORIAL_MULTIPLAYER,
  SCRIPT_GUS_DAY_1,
  SCRIPT_GALE_DAY_2,
  SCRIPT_GUS_DAY_3,
  SCRIPT_GALE_DAY_4,
  SCRIPT_GUS_DAY_5,
  SCRIPT_ENDING_GOOD,
  SCRIPT_ENDING_BAD,
  SCRIPT_GUS_ANGUISH
};

const Script SCRIPTS[64] = {
    {DUMMY,
     4,
     {
      "Anger",
      "Exclamation",
      "Question",
      "None"
      },
     {
         EMOTE_ANGRY,
         EMOTE_EXCLAMATION,
         EMOTE_QUESTION,
         EMOTE_NONE,
     }},
    {GUS,
     6,
     {
      "Walter.",
      "We have a deal.",
      "You're going to cook for me.",
      "You will do so efficiently.",
      "Three hundred pounds a week.",
      "That is your goal."
      "I suggest you get a move on."
      },
     {EMOTE_ANGRY,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_EXCLAMATION}},
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
     {
      "M-Mister White!",
      "How good it is to see you    again!",
      "I-I'd be delighted to give   you a little refresher.",
      "Allow me to take the lead!",
      "First and foremost, head to  the control valve.",
      "I've marked it on the floor  plan with a fun red X!"
      },
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
    {GALE,
     3,
     {
      "Oh, what fun! It's time to   use the pestle and mortar!",
      "Drag the pestle in an up-    and-around motion.",
      "That'll grind the solutes    to a fine powder.",
     },
     {EMOTE_EXCLAMATION,
      EMOTE_NONE,
      EMOTE_NONE}},
    {GALE,
     3,
     {
      "As expected of Mister White!",
      "The solutes are ground to    perfection.",
      "Now we can use the mixing    plant!",
      "This will mix our solution   into the methylamine base."
      },
     {EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_EXCLAMATION}},
    {GALE,
     7,
     {
      "Say hello to Sally the mixer.Isn't she a beauty?!",
      "She'll do the work for us--  all we need to do is use...",
      "...the cranks to position    the flexi-pipe.",
      "The crank on your right will adjust it vertically...",
      "And the crank on your left   will adjust it horizontally.",
      "Line up the pipe's head with the tank's coupling hole...",
      "...and it'll click into      place and start mixing!"
      },
     {EMOTE_EXCLAMATION,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_EXCLAMATION}},
    {GALE,
     4,
     {
      "Thank you, Sally!",
      "(...)",
      "...Let's move on to the next task.",
      "To the test tubes!"
      },
     {EMOTE_EXCLAMATION,
      EMOTE_QUESTION,
      EMOTE_NONE,
      EMOTE_NONE}},
    {GALE,
     7,
     {
      "Now we've mixed up the       solutes, we'll need to...",
      "add a dash of reagent to     start the reaction.",
      "Drag the pipette over each   test tube, then...",
      "...when it's in position,    release the stylus!",
      "The pipette will let out     a droplet!",
      "Make sure it's aligned with  the test tube, and the...",
      "...reagent will do the rest!"
      },
     {EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_EXCLAMATION}},
    {GALE,
     4,
     {
      "Excellent! Just look at thosetubes go.",
      "Ooh, we're nearly done!",
      "Isn't this exciting?",
      "Next up, pouring our product into proving trays!"
      },
     {EMOTE_EXCLAMATION,
      EMOTE_NONE,
      EMOTE_EXCLAMATION,
      EMOTE_NONE}},
    {GALE,
     6,
     {
      "It's pouring time!",
      "The methamphetamine solution needs pouring into trays.",
      "Tap the switch labeled \"Pump\"to start the flow.",
      "You need to fill it preciselyto the line-no more or less.",
      "Use the drain switch to lowerthe level if you need to!",
      "Tap elsewhere on the screen  to deactivate both switches."
      },
     {EMOTE_EXCLAMATION,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE}},
    {GALE,
     2,
     {
      "Excellent work! We're nearly there.",
      "The last step is fracturing  the proven methamphetamine.",
     },
     {EMOTE_NONE,
      EMOTE_EXCLAMATION}},
    {GALE,
     7,
     {
      "It's finally time for my     favorite step!",
      "Cracking the methamphetamine into crystals.",
      "You'll need to smash each    part of the tray.",
      "Once broken, slide each      crystal to below the",
      "tray to be ready for         bagging.",
      "After this step, the batch's yield will be calculated.",
      "This will be automatically   deducted from the quota.",
     },
     {EMOTE_EXCLAMATION,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE}},
    {GALE,
     6,
     {
      "You did it! Excellent work!",
      "I hope that gave you a good  refresher on the game!",
      "To finish your quota in time,you'll need be both...",
      "...quick and accurate at eachtask!",
      "I'll leave the next batch in your capable hands!",
      "Good luck, Mister White!"
      },
     {EMOTE_EXCLAMATION,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_EXCLAMATION}},
    {GUS,
     8,
     {
      "Good evening.",
      "I trust Gale's refresher was ...Useful?",
      "Allow me to remind you of an old addage:",
      "Twice the pride, double the  fall.",
      "That is of course to remind  you that with",
      "another Nintendo DS system,  you can compete",
      "with Jesse in Multiplayer    Vs. Mode.",
      "A pleasure doing business    with you as always."
      },
     {EMOTE_NONE,
      EMOTE_QUESTION,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE,
      EMOTE_NONE}},
    {GUS,
      12,
      {
      "Good morning.",
      "I trust that you are well.",
      "I have prepared a new batch  of chemicals for you.",
      "I trust you'll find them to  your satisfaction.",
      "Your quota for today is eightunits.",
      "I know you won't disappoint.",
      "Allow me to remind you of ourdeal.",
      "You shall be paid handsomely for your work.",
      "$30 per unit of product. Plusbonuses for quick delivery.",
      "Should you maintain a purity level of 100\% all day...",
      "...You shall receive some    additional compensation.",
      "Good luck."
      },
      {
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE
      }
    },
    {GALE,
      10,
      {
      "(...)",
      "M-M-Mister White!",
      "(...)",
      "I-I-Uh...Don't mind me!",
      "I'm... Just here to observe  today...",
      "(...)",
      "Misterfringtoldmetowatchyou",
      "(...)",
      "Egad! I said too much...",
      "I-I'll just be going now..."
      },
      {
        EMOTE_NONE,
        EMOTE_EXCLAMATION,
        EMOTE_NONE,
        EMOTE_EXCLAMATION,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_QUESTION,
        EMOTE_EXCLAMATION,
        EMOTE_NONE
      }
    },
    {GUS,
      8,
      {
      "Hello again.",
      "As you can see, your quota   has increased.",
      "I apologise for putting such a burden upon you...",
      "But I know you can handle it.On a separate matter...",
      "I am told you have been      consulting after work with",
      "...your lawyer.",
      "I have no qualms with this.  But I trust details of our",
      "...arrangement remain...     Confidential."
      },
      {
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE
      }
    },
    {GALE,
    8,
      {
        "M-M-M-M-M-M-M-M-M-M-M-Mister W-W-W-W-W-White...",
        "I-I-I'm just here to...      observe today...",
        "(you gotta get out of here)",
        "(the boss is coming)",
        "(tomorrow after work)",
        "(he... he...)",
        "(he's watching now, too.)",
        "(you need to come up with a  plan...)",
      },
      {
        EMOTE_EXCLAMATION,
        EMOTE_EXCLAMATION,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE
      }
    },
    {GUS,
    6,
      {
        "Good morning.",
        "After work today, I would    like to speak with you.",
        "Please make sure you are     available.",
        "I have a proposition for you.",
        "That is all.",
        "I will make my way into the  lab this evening.",
      },
      {
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE
      }
    },
    {GALE,
    13,
      {
        "M-M-Mister White!",
        "(he's coming, isn't he?)",
        "(you need to get out.)",
        "(...)",
        "(golly gosh! are those...    explosives?)",
        "(you're going to blow up the lab?!)",
        "(...)",
        "(but what about Sally...?)",
        "(...)",
        "I'll help you.",
        "But I'll need you to buy me  some time.",
        "One last batch. That's all   I need.",
        "We're in this together, sir."
      },
      {
        EMOTE_EXCLAMATION,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE
      }
    },
    {GUS,
    7,
      {
        "Walter.",
        "Have you ever heard the      expression...",
        "...\"The road to hell is pavedwith good intentions?\"",
        "I'm afraid I won't be needingyour services any more.",
        "I have nothing more to say toyou.",
        "Our arrangement is finished.",
        "Goodbye."
      },
      {
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE,
        EMOTE_NONE
      }
    },
    {GUS,
    1,
      {
        "HAAAAAAAAAAHHHHHH!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
      },
      {
        EMOTE_ANGRY
      }
    },
    };

const Portrait PORTRAITS[64] = {
    {DUMMY, "sprite/dummy", "bg/guassian", {0, 1}, {2, 3, 4, 5}},
    {GALE, "sprite/gale", "bg/lab_gale", {0, 1}, {2, 3, 4, 5}},
    {GUS, "sprite/gus", "bg/pollos_gus", {0, 1}, {2, 3, 4, 5}}
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
  int GetStoryDialogue(int dialogueProgress, int batchProgress, int batchesComplete, Tile playerTile);
};