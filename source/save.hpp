//
// Created by cervi on 24/08/2022.
//
#pragma once

#include <cstdio>
#include <nds.h>

#include "card.hpp"

#define MINERAL_SAVE_SIZE 12
#define POWER_UP_SAVE_SIZE 5 

class SaveData {
public:
    const u32 saveVersion = 1;
    void clear();
    void loadData();
    void saveData();

    bool saveExists = false;
    bool minerals[MINERAL_SAVE_SIZE] = {false};
    bool powerUps[POWER_UP_SAVE_SIZE] = {false};
    u16 currentDay = 0;
    u16 currentMoney = 0;
    u16 currentDialogue = 0;
};

extern SaveData globalSave;