#pragma once

#include <nds.h>

static void rumble(bool actuator) {
    if (!isDSiMode()) {
        setRumble(actuator);
    }
}