#pragma once

#include <cstdio>
#include <cstdlib>

#include <nds.h>
#include <nf_lib.h>
#include <maxmod9.h>

enum BgmId
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

struct Bgm
{
    const char* name;
    const char* path;
};

const int BGM_COUNT = 8;
const Bgm BGMS[BGM_COUNT] =
{
    {
        "Breaking Bad (Title)",
        "bgm/title_hook"
    },
    {
        "Breaking Bad (Menu)"
        "bgm/title_loop",
    },
    {
        "Baby Blue (Game Over)",
        "bgm/baby_blue"
    },
    {
        "The Cousins (Superlab)",
        "bgm/the_cousins"
    },
    {
        "Clear Waters (Tutorial)",
        "bgm/clear_waters"
    },
    {
        "Rodrigo y Gabriela (Clear)",
        "bgm/rodrigo_y_gabriela"
    },
    {
        "Saul's Theme",
        "bgm/sauls_theme"
    },
    {
        "The Final Cook",
        "bgm/the_final_cook"
    }
};

enum SfxId
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

const int SFX_COUNT = 12;
const char* SFXS[SFX_COUNT] =
{
    {"sfx/menu_drum"},
    {"sfx/menu_select"},
    {"sfx/dialogue_bleep"},
    {"sfx/success_bell"},
    {"sfx/valve_turn"},
    {"sfx/goodbye_walter"},
    {"sfx/pestle"},
    {"sfx/crank"},
    {"sfx/click"},
    {"sfx/vaccum"},
    {"sfx/acceptable"},
    {"sfx/minerals"}
};

namespace Audio {
    // We do not read a sample at a time, that would take too long. We load in chunks.
    // Size of the buffer:
    const int kWAVBuffer = 1000;  // in samples

    class WAV {
    public:
        int loadWAV(const char* name);
        void free_();
        
        char* getFilename() {return _filename;}
        bool getLoaded() const { return _loaded; }
        void setLoops(int loops) { _loops = loops; }
        bool getStereo() const { return _stereo; }

        bool getActive() const {return _active;}
        void play();
        void stop();

        // Sometimes we'll want to start a WAV without
        // having to keep a reference.
        // We want it to free itself once it finishes playing
        // Then we allocate it in the heap using new and
        // set tne following variable to true.
        bool deleteOnStop = false;
    private:
        char* _filename = nullptr;
        int _loops = 0;
        bool _loaded = false;
        u16 _sampleRate = 0;
        bool _stereo = false;
        u16 _bitsPerSample = 8;
        FILE* _stream = nullptr;
        u32 _dataEnd = 0;
        u32 _dataStart = 0;

        u32 _co = 44100;  // Used to linearly convert sample rate
        u16 _cValueIdx = kWAVBuffer;
        u16 _maxValueIdx = kWAVBuffer;
        u16 _values[kWAVBuffer * 2] = {0};
        bool _active = false;
        WAV* _prev = nullptr;
        WAV* _next = nullptr;
    public:
        friend mm_word fillAudioStream(mm_word, mm_addr, mm_stream_formats);
        friend bool fillAudioStreamWav(WAV*, mm_word, u16*, mm_stream_formats);
    };

    void initAudioStream();
    mm_word fillAudioStream(mm_word length, mm_addr dest, mm_stream_formats format);
    bool fillAudioStreamWav(WAV* wav, mm_word length, u16* dest, mm_stream_formats format);

    void PlayBGM(BgmId bgm, bool loop);
    void StopBGM();

    extern WAV cBGMusic;

    extern WAV* playingWavHead;
}