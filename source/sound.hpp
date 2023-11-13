// This file is licensed under the GNU GPL 3.0

//
// Created by cervi on 28/09/2022.
//
#pragma once

#include <cstdio>
#include <cstring>
#include <string>
#include <list>
#include <memory>
#include <utility>
#include <algorithm>
#include <nds.h>

struct WavFile
{
    const std::string name;
    const std::string path;
};

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

const int BGM_COUNT = 8;
const WavFile BGMS[BGM_COUNT] =
{
    {"Breaking Bad (Title)", "bgm/title_hook"},
    {"Breaking Bad (Menu)", "bgm/title_loop"},
    {"Baby Blue (Game Over)", "bgm/baby_blue"},
    {"The Cousins (Superlab)", "bgm/the_cousins"},
    {"Clear Waters (Tutorial)", "bgm/clear_waters"},
    {"Rodrigo y Gabriela (Clear)", "bgm/rodrigo_y_gabriela"},
    {"Saul's Theme", "bgm/sauls_theme"},
    {"The Final Cook", "bgm/the_final_cook"}
};

const int SFX_COUNT = 12;
const WavFile SFXS[SFX_COUNT] =
{
    {"Drum", "sfx/menu_drum"},
    {"Select", "sfx/menu_select"},
    {"Bleep", "sfx/dialogue_bleep"},
    {"Bell", "sfx/success_bell"},
    {"Turn", "sfx/valve_turn"},
    {"Waltuh", "sfx/goodbye_walter"},
    {"Pestle", "sfx/pestle"},
    {"Crank", "sfx/crank"},
    {"Click", "sfx/click"},
    {"Vaccum", "sfx/vaccum"},
    {"Acceptable", "sfx/acceptable"},
    {"Minerals", "sfx/minerals"}
};

namespace Audio 
{
    class AudioManager;

    constexpr int kAudioBuffer = 0x800;  // in samples, better if it's a power of 2

    class AudioFile {
    public:
        /**
         * Loads the audio file corresponding to the name passed as a parameter.
         * @param name The name of the audio file to load.
         */
        virtual void load(const std::string& name) = 0;

        /**
         * Plays the audio file. If it is already playing, it is first stopped and
         * then played again. Progresses the first half of the audio buffer.
         */
        void play();

        /**
         * Stops the audio file if it is playing. If the file has been setup as self-freeing,
         * then it frees itself.
         */
        void stop();

        /**
         * Gets the filename that was used when load was called.
         * @return The filename used when load was called, or the empty string if load was never called.
         */
        std::string getFilename() {return _filename;}

        /**
         * Sets the number of times to loop the sample.
         * If you call it with loops = 1, the sample will play twice (looping once).
         * If you set loops to -1, the sample will loop indefinitely.
         * @param loops The number of times to loop the sample.
         */
        void setLoops(int loops) {_loops = loops;}

        /**
         * Get whether the file is a stereo audio file.
         * @return Whether the file is stereo.
         */
        bool getStereo() const {return _stereo;}

        /**
         * Get whether the file is playing at the moment.
         * @return Whether the file is playing.
         */
        bool getPlaying() const {return _active;}

        /**
         * Get whether the file has been loaded.
         * @return Whether the file has been loaded.
         */
        bool getLoaded() const {return _loaded;}

        /**
         * Get the current volume of the audio file.
         * @return The volume of the audio (from min=0 to max=127).
         */
        u8 getVolume() const {return _volume;}

        /**
         * Set the current volume of the audio file.
         * @param volume The volume of the audio (from min=0 to max=127).
         */
        void setVolume(u8 volume);

        /**
         * Use this function to start a WAV without keeping a reference to it and having it free itself
         * when it stops playing. This function expects a shared_ptr to itself, which we'll be freed once it stops
         * playing.
         *
         * Example:
         * @code
         * std::shared_ptr<AudioFile> audio_file;
         * ...
         * audio_file.freeOnStop(audio_file);
         * audio_file.play();
         * audio_file = nullptr; // The shared_ptr will be deleted once it stops playing.
         * @endcode
         *
         * This function shouldn't be used if:
         * <ul>
         *  <li>The file is expected to loop indefinitely.</li>
         *  <li>play() is never called.</li>
         * </ul>
         * If any of the above happen and this function has been called, this could lead to a memory leak.
         *
         * @param self A shared pointer to itself.
         */
        inline void freeOnStop(std::shared_ptr<AudioFile> self) {
            selfFreeingPtr = std::move(self);
        }

        virtual ~AudioFile() {
            if (_active)
                stop();
        }
    protected:
        /**
         * Allocates the audio buffers required to play the sample. Should be called after
         * loading the audio file.
         * Frees any existing audio buffers.
         * Used the internal _bitsPerSample and _stereo values to allocate kAudioBuffer
         * samples to the needed audio buffers, so these two values should be set
         * before calling this function.
         */
        void allocateBuffers();

        /**
         * Should reset the playing audio file to the beginning of the playback, resetting
         * any needed internal values.
         */
        virtual void resetPlaying() = 0;

        int _leftChannel = -1;
        // When allocated has kAudioBuffer samples of size
        // The size of each sample is determined by _bitsPerSample.
        std::unique_ptr<u8[]> _leftBuffer = nullptr;

        int _rightChannel = -1;
        // When allocated has kAudioBuffer samples of size
        // The size of each sample is determined by _bitsPerSample.
        std::unique_ptr<u8[]> _rightBuffer = nullptr;

        // The position in the audio buffers in samples.
        u32 _sampleBufferPos;

        virtual SoundFormat getAllocFormat() = 0;

        virtual u8 getBitsPerSample() = 0;
        /**
         * Should only be called from update(). Should stream the desired samples
         * to the audio buffers.
         * @param samples The number of samples to load.
         */
        virtual void progress(u16 samples) = 0;

        std::string _filename;
        bool _active = false;
        bool _stereo = false;
        bool _loaded = false;
        int _loops = 0;  // TODO: Maybe copy to internal value when played?
        u32 _expectedSampleBufferPos;
        u16 _sampleRate = 1;
        u8 _volume = 127;

        std::shared_ptr<AudioFile> selfFreeingPtr = nullptr;

    private:
        /**
         * Should be called in every frame to stream the audio to the buffers.
         * Is called by the AudioManager.
         */
        void update();
        u16 _timerLast;
        u32 _ticksRemain;

        friend class AudioManager;
    };

    class AudioManager {
    public:
        /**
         * Initialize the audio system. Instruct the audio system to use
         * timerId as the timer for timing audio sample loading.
         * @param timerId The id of the timer to use.
         */
        explicit AudioManager(int timerId);

        /**
         * Should be called in every frame.
         * through all the currently playing audio files and updates them.
         */
        void update();

        /**
         * Gets the timer used by the audio system, specified when the audio manager is constructed.
         * @return The timer used by the audio system.
         */
        u8 getTimerId() const {return _timerId;}
    private:
        /**
         * Adds audio_file to the list of currently playing files.
         * Called internally by AudioFile when it plays.
         *
         * @param audioFile The audio file to add to the list.
         */
        void addPlaying(AudioFile* audioFile);

        /**
         * Removes audio_file from the list of currently playing files.
         * Called internally by AudioFile when it stops.
         *
         * @param audioFile The audio file to add to the list.
         */
        void removePlaying(AudioFile* audioFile);

        std::list<AudioFile*> _playing;
        u8 _timerId;

        friend class AudioFile;
    };

    void PlayBGM(BgmId bgm, bool loop);
    void StopBGM();
    std::string GetBgmStatusString();

    void LoadSFXs();
    void PlaySFX(SfxId sfx);

    extern AudioManager audioManager;
}
