#include "sound.h"

Sound::Sound()
{
}

void Sound::LoadSound()
{
    // Prepare sound engine
    soundEnable();
    NF_InitRawSoundBuffers();

    // Load raw sounds
    int currentSlot = 0;

    // Load BGMS
    for (int i = 0; i < BGM_COUNT; i++)
    {
        for (int j = 0; j < BGMS[i].fileCount; j++)
        {
            NF_LoadRawSound(BGMS[i].fileNames[j], currentSlot, SAMPLE_RATE, 0);
            currentSlot++;
        }
    }

    // Load SFX
    for (int i = 0; i < SFX_COUNT; i++)
    {
        NF_LoadRawSound(SFXS[i].fileName, currentSlot, SAMPLE_RATE, 0);
        currentSlot++;
    }
}

int Sound::GetTrackFileCount(TrackId bgm)
{
    int files = 0;
    for (int i = 0; i < BGM_COUNT; i++)
    {
        for (int j = 0; j < BGMS[i].fileCount; j++)
        {
            if (BGMS[i].track == bgm)
            {
                files++;
            }
        }
    }
    return files;
}

int Sound::GetSoundRamAddress(int id, bool sfx)
{
    int currentSlot = 0;
    for (int i = 0; i < BGM_COUNT; i++)
    {
        for (int j = 0; j < BGMS[i].fileCount; j++)
        {
            if ((BGMS[i].track == id) && !sfx)
            {
                return currentSlot;
            }
            currentSlot++;
        }
    }
    
    if (sfx)
    {
        for (int i = 0; i < SFX_COUNT; i++)
        {
            if (SFXS[i].effect == id)
            {
                return currentSlot;
            }
            currentSlot++;
        }
    }
    return -1;
}

BGM Sound::GetBGM(TrackId bgm)
{
    for (int i = 0; i < BGM_COUNT; i++)
    {
        if (BGMS[i].track == bgm)
        {
            return BGMS[i];
        }
    }
    return BGMS[0];
}

void Sound::PlayBGM(TrackId bgm, bool loop)
{
    StopBGM();
    currentBgm = bgm;
    looping = loop;
    singleFile = (looping && GetTrackFileCount(BGMS[currentBgm].track) == 1);
    currentBgmFile = 0;
    currentBgmFrame = 0;
}

void Sound::Update(volatile int frame)
{
    if (currentBgmFrame >= BGMS[currentBgm].trackFrames[currentBgmFile])
    {
        if (!singleFile)
        {
            currentBgmFrame = 0;
            currentBgmFile++;
            if (currentBgmFile >= GetTrackFileCount(BGMS[currentBgm].track))
            {
                currentBgmFile = BGMS[currentBgm].loopAfterFile;
            }
        }
        else
        {
            currentBgmFrame = 1;
        }
    }
    if (currentBgmFrame == 0)
    {
	    currentBgmSoundId = NF_PlayRawSound(
            (GetSoundRamAddress(BGMS[currentBgm].track, false) + currentBgmFile), 
            BGMS[currentBgm].volume,
            SOUND_PAN,
            singleFile && looping,
            0
        );
    }
    currentBgmFrame++;
}

char* Sound::GetBgmTrackProgressString()
{
    // return a string in the format xx:xx. Assume 60fps.
    const int trackFiles = GetTrackFileCount(currentBgm);
    int frames = 0;
    int totalFrames = 0;
    for (int i = 0; (i < trackFiles); i++)
    {
        int trackFrames = BGMS[currentBgm].trackFrames[i];
        if (i < currentBgmFile)
        {
            frames += trackFrames;
        }
        totalFrames += trackFrames;
    }
    frames += currentBgmFrame;

    int seconds = frames / 60;
    int totalSeconds = totalFrames / 60;
    int minutes = seconds / 60;
    int totalMinutes = totalSeconds / 60;
    seconds = seconds % 60;
    totalSeconds = totalSeconds % 60;
    char* progressString = new char[6];
    sprintf(progressString, "%02d:%02d/%02d:%02d", minutes, seconds, totalMinutes, totalSeconds);
    return progressString;
}

void Sound::StopBGM()
{
    soundKill(currentBgmSoundId);
    looping = false;
}

void Sound::PlaySFX(EffectId sfx)
{
    currentSfxSoundId = NF_PlayRawSound(
        GetSoundRamAddress(sfx, true),
        127,
        SOUND_PAN,
        false,
        0
    );
}

void Sound::StopSFX()
{
    soundKill(currentSfxSoundId);
}