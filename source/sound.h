#pragma once

#include <nds.h>
#include <nf_lib.h>

enum BGM
{
    BGM_TITLE_HOOK,
    BGM_TITLE_LOOP
};

enum SFX
{

};

class Sound
{
private:
    const int BGM_CHANNEL = 0;
    const int BGM_VOLUME = 127;
    const int BGM_PAN = 64;
    const int BGM_SAMPLE_RATE = 22050;
public:
    Sound();
    void LoadSound();
    void PlayBGM(BGM bgm, bool loop, int loopAfter);
    void StopBGM();
    void PlaySFX(SFX sfx);
    void StopSFX();
};