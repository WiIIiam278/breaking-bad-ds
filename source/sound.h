#pragma once

#include <cstdio>
#include <cstdlib>

#include <nds.h>
#include <nf_lib.h>

enum TrackId
{
    BGM_TITLE_INTRO,
    BGM_TITLE_LOOP,
    BGM_BABY_BLUE,
    BGM_THE_COUSINS
};

struct BGM
{
    const char name[64];
    const TrackId track;
    const int volume;
    const int fileCount;
    const char fileNames[8][64];
    const int trackFrames[8];
    const int loopAfterFile;
};

const int BGM_COUNT = 4;
const BGM BGMS[BGM_COUNT] =
    {
        {"Main Theme (Intro)",
         BGM_TITLE_INTRO,
         127,
         1,
         {"bgm/title_hook"},
         {690},
         0},
        {"Main Theme (Loop)",
         BGM_TITLE_LOOP,
         127,
         1,
         {"bgm/title_loop"},
         {640},
         0},
        {"Baby Blue (Game Over Theme)",
         BGM_BABY_BLUE,
         127,
         1,
         {"bgm/baby_blue"},
         {640},
         0},
        {"The Cousins (Lab Theme)",
         BGM_THE_COUSINS,
         77,
         8,
         {"bgm/the_cousins_hook_1",
          "bgm/the_cousins_hook_2",
          "bgm/the_cousins_hook_3",
          "bgm/the_cousins_hook_4",
          "bgm/the_cousins_hook_5",
          "bgm/the_cousins_hook_6",
          "bgm/the_cousins_loop_1",
          "bgm/the_cousins_loop_2"},
         {585, 585, 585, 585, 585, 585, 585, 585},
         3}};

enum EffectId
{
    SFX_MENU_DRUM,
    SFX_MENU_SELECT,
    SFX_DIALOGUE_BLEEP,
    SFX_SUCCESS_BELL,
    SFX_VALVE_TURN
};

struct SFX
{
    const EffectId effect;
    const char fileName[64];
};

const int SFX_COUNT = 5;
const SFX SFXS[SFX_COUNT] =
    {
        {SFX_MENU_DRUM, "sfx/menu_drum"},
        {SFX_MENU_SELECT, "sfx/menu_select"},
        {SFX_DIALOGUE_BLEEP, "sfx/dialogue_bleep"},
        {SFX_SUCCESS_BELL, "sfx/success_bell"},
        {SFX_VALVE_TURN, "sfx/valve_turn"},
    };

class Sound
{
private:
    const int BGM_CHANNEL = 0;
    const int BGM_VOLUME = 127;
    const int SOUND_PAN = 64;
    const int SAMPLE_RATE = 22050;

    TrackId currentBgm;
    int currentBgmFile;
    volatile int currentBgmFrame;
    bool singleFile = false;
    bool looping = false;

    u32 currentBgmSoundId = 0;
    u32 currentSfxSoundId = 0;

public:
    Sound();
    void LoadSound();
    int GetSoundRamAddress(int bgm, bool sfx);
    int GetTrackFileCount(TrackId bgm);
    BGM GetBGM(TrackId bgm);
    void PlayBGM(TrackId bgm, bool loop);
    void Update(volatile int frame);
    char *GetBgmTrackProgressString();
    void StopBGM();
    void PlaySFX(EffectId sfx);
    void StopSFX();
};