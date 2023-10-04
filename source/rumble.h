#pragma once

#include <nds.h>

static bool canRumble = false;

static void rumbleInit() {
    canRumble = !isDSiMode() && isRumbleInserted();
}

static void rumble(bool actuator) {
    if (canRumble) {
        setRumble(actuator);
    }
}

static bool readyToRumble() {
    return canRumble;
}