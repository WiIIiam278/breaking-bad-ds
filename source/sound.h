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
    BGM_THE_COUSINS,
    BGM_CLEAR_WATERS,
    BGM_RODRIGO_Y_GABRIELA,
    BGM_SAUL,
    BGM_FINAL_COOK
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

const int BGM_COUNT = 8;
const BGM BGMS[BGM_COUNT] =
    {
        {"Breaking Bad (Title)",
         BGM_TITLE_INTRO,
         127,
         1,
         {"bgm/title_hook"},
         {690},
         0},
        {"Breaking Bad (Menu)",
         BGM_TITLE_LOOP,
         127,
         1,
         {"bgm/title_loop"},
         {640},
         0},
        {"Baby Blue (Game Over)",
         BGM_BABY_BLUE,
         127,
         1,
         {"bgm/baby_blue"},
         {640},
         0},
        {"The Cousins (Superlab)",
         BGM_THE_COUSINS,
         80,
         2,
         {"bgm/the_cousins_1",
          "bgm/the_cousins_2"},
         {585, 585},
         0},
        {
            "Clear Waters (Tutorial)",
            BGM_CLEAR_WATERS,
            100,
            2,
            {"bgm/clear_waters_1",
             "bgm/clear_waters_2"},
            {645, 620},
            1
        },
        {
            "Rodrigo y Gabriela (Clear)",
            BGM_RODRIGO_Y_GABRIELA,
            100,
            1,
            {"bgm/rodrigo_y_gabriela"},
            {640},
            0
        },
        {
            "Saul's Theme",
            BGM_SAUL,
            127,
            1,
            {"bgm/sauls_theme"},
            {300},
            0
        },
        {
            "The Final Cook",
            BGM_FINAL_COOK,
            127,
            1,
            {"bgm/the_final_cook"},
            {620},
            0
        }
    };

enum EffectId
{
    SFX_MENU_DRUM,
    SFX_MENU_SELECT,
    SFX_DIALOGUE_BLEEP,
    SFX_SUCCESS_BELL,
    SFX_VALVE_TURN,
    SFX_GOODBYE_WALTER,
    SFX_PESTLE,
    SFX_CRANK,
    SFX_CLICK,
    SFX_VACCUM,
    SFX_ACCEPTABLE,
    SFX_MINERALS
};

struct SFX
{
    const EffectId effect;
    const char fileName[64];
};

const int SFX_COUNT = 12;
const SFX SFXS[SFX_COUNT] =
    {
        {SFX_MENU_DRUM, "sfx/menu_drum"},
        {SFX_MENU_SELECT, "sfx/menu_select"},
        {SFX_DIALOGUE_BLEEP, "sfx/dialogue_bleep"},
        {SFX_SUCCESS_BELL, "sfx/success_bell"},
        {SFX_VALVE_TURN, "sfx/valve_turn"},
        {SFX_GOODBYE_WALTER, "sfx/goodbye_walter"},
        {SFX_PESTLE, "sfx/pestle"},
        {SFX_CRANK, "sfx/crank"},
        {SFX_CLICK, "sfx/click"},
        {SFX_VACCUM, "sfx/vaccum"},
        {SFX_ACCEPTABLE, "sfx/acceptable"},
        {SFX_MINERALS, "sfx/minerals"}
    };

class Sound
{
private:
    const int BGM_CHANNEL = 0;
    const int BGM_VOLUME = 127;
    const int SOUND_PAN = 64;
    const int SAMPLE_RATE = 22050;

    TrackId currentBgm;
    bool bgmPlaying = false;
    int currentBgmFile;
    volatile int currentBgmFrame;
    bool singleFile = false;
    bool looping = false;

    char* progressString;

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