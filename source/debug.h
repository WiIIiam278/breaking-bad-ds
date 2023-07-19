#pragma once

#include <malloc.h>
#include <unistd.h>
#include <nds.h>

u8* getHeapStart();
u8* getHeapEnd();
u8* getHeapLimit();
int getMemUsed();
int getMemFree();