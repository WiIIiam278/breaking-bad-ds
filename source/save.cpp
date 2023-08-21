#include "save.h"

void SaveGame(SaveFile *saveFile, const char* fileName)
{
    FILE *file = fopen(fileName, "wb");
    if (file == nullptr)
    {
        return;
    }

    fwrite(saveFile, sizeof(SaveFile), 1, file);
    fclose(file);
}

void LoadGame(SaveFile *saveFile, const char* fileName)
{
    FILE *file = fopen(fileName, "rb");
    if (file == nullptr)
    {
        SaveGame(saveFile, fileName);
        return;
    }

    fread(saveFile, sizeof(SaveFile), 1, file);
    fclose(file);
    
    if (saveFile->version != SAVE_FILE_VERSION)
    {
        saveFile = CreateNewSaveFile();
        SaveGame(saveFile, fileName);
    }
}

SaveFile *CreateNewSaveFile()
{
    SaveFile *saveFile = new SaveFile();

    // Header
    saveFile->version = SAVE_FILE_VERSION;
    for (int i = 0; i < 4; i++)
    {
        saveFile->fileHeader[i] = SAVE_FILE_HEADER[i];
    }
    
    // Story mode
    saveFile->storyModeDay = 0;
    saveFile->storyModeMoney = 0;
    saveFile->storyModeDialogueProgress = saveFile->storyModeDay;
    for (int p = 0; p < POWER_UP_COUNT; p++)
    {
        saveFile->storyModePowerUps[p] = false;
    }
    
    // Minerals
    for (int m = 0; m < MINERAL_COUNT; m++)
    {
        saveFile->minerals[m] = false;
    }
    return saveFile;
}