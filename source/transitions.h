#pragma once

#include <nds.h>

static bool isTransitioning[2] = { false, false };
static bool isFadingIn[2] = { false, false };
static int transitionDuration[2] = { 0, 0 };
static int transitionStartFrame[2] = { 0, 0 };

enum TransitionScreen
{
    TS_TOP = 0,
    TS_BOTTOM = 1,
    TS_BOTH = 2
};

void Transition(bool fadeIn, int duration, TransitionScreen screen, volatile int frame);
void UpdateTransitions(volatile int frame);