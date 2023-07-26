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
    const char fileNames[64][64];
    const int trackFrames[64];
    const int loopAfterFile;
};

const int BGM_COUNT = 4;
const BGM BGMS[BGM_COUNT] =
{
    {
        "Main Theme (Intro)",
        BGM_TITLE_INTRO,
        127,
        { "bgm/title_hook", "\0" },
        { 690 },
        0
    },
    {
        "Main Theme (Loop)",
        BGM_TITLE_LOOP,
        127,
        { "bgm/title_loop", "\0" },
        { 640 },
        0
    },
    {
        "Baby Blue (Game Over Theme)",
        BGM_BABY_BLUE,
        127,
        { "bgm/baby_blue", "\0" },
        { 640 },
        0
    },
    {
        "The Cousins (Lab Theme)",
        BGM_THE_COUSINS,
        42,
        { 
            "bgm/the_cousins_hook_1", 
            "bgm/the_cousins_hook_2",
            "bgm/the_cousins_hook_3",
            "bgm/the_cousins_hook_4",
            "bgm/the_cousins_hook_5",
            "bgm/the_cousins_hook_6",
            "bgm/the_cousins_loop_1",
            "bgm/the_cousins_loop_2", 
            "\0"
        },
        { 585, 585, 585, 585, 585, 585, 585, 585 },
        3
    }
};

enum EffectId
{

};

struct SFX {
    const char name[64];
    const EffectId effect;
    const char fileName[64];
};

class Sound
{
private:
    const int BGM_CHANNEL = 0;
    const int BGM_VOLUME = 127;
    const int BGM_PAN = 64;
    const int BGM_SAMPLE_RATE = 22050;

    TrackId currentBgm;
    int currentBgmFile;
    volatile int currentBgmFrame;
    bool singleFile = false;
    bool looping = false;
public:
    Sound();
    void LoadSound();
    int GetFirstRamAddressFor(TrackId bgm);
    int GetTrackFileCount(TrackId bgm);
    BGM GetBGM(TrackId bgm);
    void PlayBGM(TrackId bgm, bool loop);
    void Update(volatile int frame);
    char* GetProgressString();
    void StopBGM();
    void PlaySFX(EffectId sfx);
    void StopSFX();
};