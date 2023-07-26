#include "sound.h"

Sound::Sound()
{
}

void Sound::LoadSound()
{
    // Prepare sound engine
    soundEnable();
    NF_InitRawSoundBuffers();

    // Load sound
    int currentSlot = 0;
    for (int i = 0; i < BGM_COUNT; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            if (BGMS[i].fileNames[j][0] == '\0')
            {
                break;
            }

            NF_LoadRawSound(BGMS[i].fileNames[j], currentSlot, BGM_SAMPLE_RATE, 0);
            currentSlot++;
        }
    }
}

int Sound::GetTrackFileCount(TrackId bgm)
{
    int files = 0;
    for (int i = 0; i < BGM_COUNT; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            if (BGMS[i].fileNames[j][0] == '\0')
            {
                break;
            }

            if (BGMS[i].track == bgm)
            {
                files++;
            }
        }
    }
    return files;
}

int Sound::GetFirstRamAddressFor(TrackId bgm)
{
    int currentSlot = 0;
    for (int i = 0; i < BGM_COUNT; i++)
    {
        for (int j = 0; j < BGM_COUNT; j++)
        {
            if (BGMS[i].fileNames[j][0] == '\0')
            {
                break;
            }

            if (BGMS[i].track == bgm)
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
    currentBgmFrame = 0;
    currentBgmFile = 0;
    singleFile = (looping && GetTrackFileCount(BGMS[currentBgm].track) == 1);
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
	    NF_PlayRawSound(
            (GetFirstRamAddressFor(BGMS[currentBgm].track) + currentBgmFile), 
            BGMS[currentBgm].volume,
            BGM_PAN,
            singleFile && looping,
            0
        );
    }
    currentBgmFrame++;
}

char* Sound::GetProgressString()
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
    soundKill(0);
}

void Sound::PlaySFX(EffectId sfx)
{
    StopSFX();
    // Play sound effect
}

void Sound::StopSFX()
{
    soundKill(0);
}