//
// Created by cervi on 24/08/2022.
//
#include "save.hpp"

SaveData globalSave;

void SaveData::clear() {
    memset(minerals, 0, MINERAL_SAVE_SIZE + 1);
    memset(powerUps, 0, POWER_UP_SAVE_SIZE + 1);
    currentDay = 0;
    currentMoney = 0;
    currentDialogue = 0;
}

void SaveData::loadData() {
    char header[4];
    char expectedHeader[4] = {'M', 'E', 'T', 'H'};

    printf("state 0\n");
    fCard.open("rb");
    printf("open\n");
    fCard.seek(0, SEEK_SET);
    printf("seek\n");
    fCard.read(header, 4);
    printf("read\n");

    if (memcmp(header, expectedHeader, 4) != 0) {
        printf("state 1\n");
        fCard.close();
        clear();
        return;
    }

    u32 saveVersion_;
    fCard.read(&saveVersion_, 4);
    if (saveVersion_ != saveVersion) {
        printf("state 2\n");
        fCard.close();
        clear();
        return;
    }
    printf("state 3\n");
    
    fCard.read(minerals, 2 * MINERAL_SAVE_SIZE);
    fCard.read(powerUps, 2 * POWER_UP_SAVE_SIZE);
    fCard.read(&currentDay, 4);
    fCard.read(&currentMoney, 4);
    fCard.read(&currentDialogue, 4);
    printf("state 4\n");
    
    saveExists = true;
    fCard.close();
}

void SaveData::saveData() {
    char header[4] = {'M', 'E', 'T', 'H'};

    fCard.open("wb");
    fCard.seek(0, SEEK_SET);
    fCard.write(header, 4);
    u32 saveVersion_ = saveVersion;
    fCard.write(&saveVersion_, 4);
    fCard.write(minerals, 2 * MINERAL_SAVE_SIZE);
    fCard.write(powerUps, 2 * POWER_UP_SAVE_SIZE);
    fCard.write(&currentDay, 4);
    fCard.write(&currentMoney, 4);
    fCard.write(&currentDialogue, 4);

    saveExists = true;
    fCard.close();
}