#include "transitions.h"

void Transition(bool fadeIn, int duration, TransitionScreen screen, volatile int frame)
{
    for (int i = 0; i < 2; i++)
    {
        if (!(screen == i || screen == TS_BOTH))
        {
            continue;
        }
        if (duration <= 0)
        {
            setBrightness(i + 1, fadeIn ? 0 : -16);
            isTransitioning[i] = false;
            continue;
        }

        isTransitioning[i] = true;
        isFadingIn[i] = fadeIn;
        transitionDuration[i] = duration;
        transitionStartFrame[i] = frame;
    }
}

void UpdateTransitions(volatile int frame)
{
    for (int i = 0; i < 2; i++)
    {
        if (!isTransitioning[i])
        {
            continue;
        }

        if (frame >= (transitionStartFrame[i] + transitionDuration[i]))
        {
            setBrightness(i + 1, isFadingIn[i] ? 0 : -16);
            isTransitioning[i] = false;
            return;
        }

        int brightness;
        if (!isFadingIn[i])
        {
            brightness = (frame - transitionStartFrame[i]) * 16 / transitionDuration[i];
        }
        else
        {
            brightness = 16 - ((frame - transitionStartFrame[i]) * 16 / transitionDuration[i]);
        }
        setBrightness(i + 1, -brightness);
    }
}