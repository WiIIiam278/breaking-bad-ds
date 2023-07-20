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
    NF_LoadRawSound("bgm/title_hook", BGM_TITLE_HOOK, BGM_SAMPLE_RATE, 0);
    NF_LoadRawSound("bgm/title_loop", BGM_TITLE_LOOP, BGM_SAMPLE_RATE, 0);
}

void Sound::PlayBGM(BGM bgm, bool loop, int loopAfter)
{
    StopBGM();
	NF_PlayRawSound(bgm, BGM_VOLUME, BGM_PAN, loop, loopAfter);
}

void Sound::StopBGM()
{
    soundKill(0);
}

void Sound::PlaySFX(SFX sfx)
{
    StopSFX();
    // Play sound effect
}

void Sound::StopSFX()
{
    soundKill(0);
}