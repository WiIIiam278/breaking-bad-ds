#include "menu.h"

Menu::Menu()
{
    state = MENU_LOADING;
    highlightedItem = NONE;
}

int Menu::Load()
{
    skybox = NE_ModelCreate(NE_Static);
    skyboxMaterial = NE_MaterialCreate();

    logo = NE_ModelCreate(NE_Static);
    logoMaterial = NE_MaterialCreate();

    text = NE_ModelCreate(NE_Static);
    textMaterial = NE_MaterialCreate();

    // Load assets from the filesystem
    if (NE_ModelLoadStaticMeshFAT(skybox, "model/logo_skybox.dl") == 0 || NE_ModelLoadStaticMeshFAT(logo, "model/logo.dl") == 0 || NE_ModelLoadStaticMeshFAT(text, "model/logo_text.dl") == 0)
    {
        consoleDemoInit();
        printf("Couldn't load logo mesh/text plane...");
        return -1;
    }
    if (NE_MaterialTexLoadFAT(skyboxMaterial, NE_A1RGB5, 256, 256, NE_TEXGEN_TEXCOORD, "model/logo_skybox_tex.bin") == 0 || NE_MaterialTexLoadFAT(logoMaterial, NE_A1RGB5, 128, 128, NE_TEXGEN_TEXCOORD, "model/logo_tex.bin") == 0 || NE_MaterialTexLoadFAT(textMaterial, NE_A1RGB5, 128, 128, NE_TEXGEN_TEXCOORD, "model/logo_text_tex.bin") == 0)
    {
        consoleDemoInit();
        printf("Couldn't load logo/text textures...");
        return -1;
    }

    // Assign material to the model
    NE_ModelSetMaterial(skybox, skyboxMaterial);
    NE_ModelSetMaterial(logo, logoMaterial);
    NE_MaterialSetPropierties(logoMaterial,
                              RGB15(24, 24, 24), // Diffuse
                              RGB15(16, 16, 16), // Ambient
                              RGB15(8, 8, 8),    // Specular
                              NE_White,          // Emission
                              false, false);     // Vertex color, use shininess table
    NE_ModelSetMaterial(text, textMaterial);
    NE_MaterialSetPropierties(textMaterial,
                              RGB15(24, 24, 24), // Diffuse
                              RGB15(16, 16, 16), // Ambient
                              RGB15(8, 8, 8),    // Specular
                              NE_White,          // Emission
                              false, false);     // Vertex color, use shininess table
    NE_ModelSetMaterial(skybox, skyboxMaterial);
    NE_MaterialSetPropierties(skyboxMaterial,
                              RGB15(0, 0, 0),    // Diffuse
                              RGB15(12, 12, 12), // Ambient
                              RGB15(8, 8, 8),    // Specular
                              NE_White,          // Emission
                              false, false);     // Vertex color, use shininess table

    // Set model rotation and scale
    int scale = 8250;
    NE_ModelScaleI(skybox, scale * 8, scale * 8, scale * 8);
    NE_ModelScaleI(logo, scale, scale, scale);
    NE_ModelScaleI(text, scale, scale, scale);

    // Position models
    x = TARGET_X - 10;
    textX = TARGET_X - 16;
    NE_ModelSetCoordI(skybox, 0, 0, 0);
    NE_ModelSetCoordI(logo, 0, 0, 0);
    NE_ModelSetCoordI(text, 0, 0, 0);
    NE_ModelTranslate(skybox, SKYPOS_POS[0], SKYPOS_POS[1], SKYPOS_POS[2]);
    NE_ModelTranslate(logo, x, Y, Z);
    NE_ModelTranslate(text, textX, Y + 0.1, Z + 4.55);

    // Mark meshes for removal when deleted
    NE_ModelFreeMeshWhenDeleted(skybox);
    NE_ModelFreeMeshWhenDeleted(logo);
    NE_ModelFreeMeshWhenDeleted(text);

    // Prepare sprites
    LoadInterfaceSprites();

    return 0;
}

void Menu::LoadInterfaceSprites()
{
    NF_LoadSpriteGfx(MENU_MISC_SPRITE_NAME, 1, 64, 32);
    NF_LoadSpritePal(MENU_MISC_SPRITE_NAME, 1);
    NF_VramSpriteGfx(1, 1, 0, false);
    NF_VramSpritePal(1, 1, 0);

    // Create, position & scale sprite
    NF_CreateSprite(1, START_SPRITE, 0, 0, 64, 64);
    NF_EnableSpriteRotScale(1, START_SPRITE, START_SPRITE, true);
    NF_SpriteRotScale(1, START_SPRITE, 0, 283, 283);

    // Load UI sprites
    NF_LoadSpriteGfx(MENU_BUTTONS_SPRITE_NAME, BUTTON_GFX_ID, MENU_SPRITE_SIZE[0], MENU_SPRITE_SIZE[1]);
    NF_LoadSpritePal(MENU_BUTTONS_SPRITE_NAME, BUTTON_GFX_ID);
    NF_VramSpriteGfx(1, BUTTON_GFX_ID, BUTTON_GFX_ID, false);
    NF_VramSpritePal(1, BUTTON_GFX_ID, BUTTON_GFX_ID);

    NF_LoadSpriteGfx(MENU_TEXT_SPRITE_NAME, TEXT_GFX_ID, MENU_SPRITE_SIZE[0], MENU_SPRITE_SIZE[1]);
    NF_LoadSpritePal(MENU_TEXT_SPRITE_NAME, TEXT_GFX_ID);
    NF_VramSpriteGfx(1, TEXT_GFX_ID, TEXT_GFX_ID, false);
    NF_VramSpritePal(1, TEXT_GFX_ID, TEXT_GFX_ID);
}

void Menu::UnloadInterfaceSprites()
{
    NF_DeleteSprite(1, START_SPRITE);
    NF_UnloadSpriteGfx(1);
    NF_UnloadSpritePal(1);
    NF_FreeSpriteGfx(1, 0);
    NF_UnloadSpriteGfx(BUTTON_GFX_ID);
    NF_UnloadSpritePal(BUTTON_GFX_ID);
    NF_FreeSpriteGfx(1, BUTTON_GFX_ID);
    NF_UnloadSpriteGfx(TEXT_GFX_ID);
    NF_UnloadSpritePal(TEXT_GFX_ID);
    NF_FreeSpriteGfx(1, TEXT_GFX_ID);
}

void Menu::ShowLayout()
{
    if (currentLayout != nullptr)
    {
        if (currentLayout->buttonCount > 0)
        {
            for (int i = 0; i < currentLayout->buttonCount; i++)
            {
                NF_DeleteSprite(1, TEXT_SPRITES[i]);
                for (int j = 0; j < 4; j++)
                {
                    NF_DeleteSprite(1, BUTTON_SPRITES_BASE + (i * 4) + j);
                }
            }
        }

        if (currentLayout->titleSprite != -1)
        {
            NF_DeleteSprite(1, TITLE_SPRITE);
        }
    }

    Layout *layout = GetLayoutForState(state);
    if (layout == nullptr)
    {
        currentLayout = nullptr;
        return;
    }

    menuCaret[0] = menuCaret[1] = 0;
    if (layout->allowCaretNavigation && layout->buttonCount > 1)
    {
        highlightedItem = layout->caretMap[0][0];
    }

    if (layout->buttonCount > 0)
    {
        for (int b = 0; b < layout->buttonCount; b++)
        {
            int spriteOffset = b * 4;
            Button button = layout->buttons[b];
            for (int i = 0; i < 4; i++)
            {
                int xOffset = (i == 1 || i == 3) ? 64 : 0;
                int yOffset = (i == 2 || i == 3) ? 32 : 0;
                int spriteId = BUTTON_SPRITES_BASE + (spriteOffset + i);
                NF_CreateSprite(1, spriteId, BUTTON_GFX_ID, BUTTON_GFX_ID, layout->buttonCoords[b][0] + xOffset, layout->buttonCoords[b][1] + yOffset);
                NF_HflipSprite(1, spriteId, i % 2 != 0);
                NF_VflipSprite(1, spriteId, i > 1);
                NF_SpriteFrame(1, spriteId, button.buttonSprite);
            }
            NF_CreateSprite(1, TEXT_SPRITES[b], TEXT_GFX_ID, TEXT_GFX_ID, layout->buttonCoords[b][0] + 32, layout->buttonCoords[b][1] + 16);
            NF_SpriteFrame(1, TEXT_SPRITES[b], button.textSprite);
        }
    }

    if (layout->titleSprite != -1)
    {
        NF_CreateSprite(1, TITLE_SPRITE, TEXT_GFX_ID, TEXT_GFX_ID, layout->titleCoords[0], layout->titleCoords[1]);
        NF_SpriteFrame(1, TITLE_SPRITE, layout->titleSprite);
    }

    currentLayout = layout;
}

void Menu::UpdateLayout(volatile int frame)
{
    const Layout *layout = GetLayoutForState(state);
    if (layout == nullptr)
    {
        return;
    }

    for (int b = 0; b < layout->buttonCount; b++)
    {
        int spriteOffset = b * 4;
        Button button = layout->buttons[b];
        for (int i = 0; i < 4; i++)
        {
            int spriteId = BUTTON_SPRITES_BASE + (spriteOffset + i);
            NF_SpriteFrame(1, spriteId, button.buttonSprite + (highlightedItem == button.selection ? 4 : 0));
        }
    }
}

void Menu::ShowBackground()
{
    if ((state == MENU_LOGO || state == MENU_LOADING))
    {
        if (currentBackground != 0)
        {
            NF_DeleteTiledBg(1, MENU_BG_ID);
            NF_UnloadTiledBg(currentBgName);
        }
        currentBackground = 0;
    }
    else
    {
        Layout *layout = GetLayoutForState(state);
        if (layout == nullptr)
        {
            return;
        }

        if (currentBackground != layout->backgroundId)
        {
            if (currentBackground != 0)
            {
                NF_DeleteTiledBg(1, MENU_BG_ID);
                NF_UnloadTiledBg(currentBgName);
            }
            currentBackground = layout->backgroundId;
            currentBgName = layout->backgroundName;
            NF_LoadTiledBg(currentBgName, currentBgName, 256, 256);
            NF_CreateTiledBg(1, MENU_BG_ID, currentBgName);
        }
    }
}

void Menu::SetState(MenuState newState, volatile int frame, Sound *sound, SaveFile *saveFile)
{
    Layout *newLayout = GetLayoutForState(newState);
    bool transitionNeeded = false;
    if ((newLayout != nullptr) && (transitionNeeded = (newLayout->backgroundId != currentBackground)))
    {
        Transition(false, 0, TS_BOTTOM, frame);
    }

    if (newState == MENU_LOADING)
    {
        sound->StopBGM();
    }
    else if (state == MENU_LOGO && newState != MENU_LOGO)
    {
        sound->PlayBGM(BGM_TITLE_LOOP, true);
    }
    else if (newState == MENU_LOGO)
    {
        sound->PlayBGM(BGM_TITLE_INTRO, false);
    }

    if (state == MENU_MUSIC_PLAYER && newState == MENU_MAIN)
    {
        sound->PlayBGM(BGM_TITLE_LOOP, true);
        currentSoundTestTrack = 1;
    }

    if ((state == MENU_MP_HOST_ROOM || state == MENU_MP_JOIN_ROOM) && (newState != MENU_MP_HOST_ROOM && newState != MENU_MP_JOIN_ROOM))
    {
        ShowMultiplayerStatus(false);
    }
    else if (newState == MENU_MP_HOST_ROOM)
    {
        StartMultiplayer(true);
    }
    else if (newState == MENU_MP_JOIN_ROOM)
    {
        StartMultiplayer(false);
    }

    if (newState == MENU_MINERALS && state != MENU_MINERALS)
    {
        ShowMinerals(true, saveFile);
    }
    else if (state == MENU_MINERALS && newState != MENU_MINERALS)
    {
        ShowMinerals(false, saveFile);
    }

    if (state != MENU_LOGO && newState != MENU_LOGO)
    {
        sound->PlaySFX(SFX_MENU_DRUM);
        rumble(state % 2 == 0);
    }
    state = newState;
    ShowLayout();
    ShowBackground();

    if (transitionNeeded)
    {
        Transition(true, 15, TS_BOTTOM, frame);
    }
}

void Menu::PositionLogo()
{
    // Position logo
    NE_ModelSetCoordI(logo, 0, 0, 0);
    NE_ModelSetCoordI(text, 0, 0, 0);
    NE_ModelTranslate(logo, TARGET_X, Y, Z);
    NE_ModelTranslate(text, TARGET_TEXT_X, Y + 0.1, Z + 4.55);
}

void Menu::StartMultiplayer(bool mpCreatingRoom)
{
    this->mpCreatingRoom = mpCreatingRoom;
    joinMultiplayer(mpCreatingRoom);
}

void Menu::UpdateMultiplayer()
{
    tickMultiplayer();
    mpCurrentStatus = getMultiplayerStatus();
    ShowMultiplayerStatus(true);
}

void Menu::ShowMultiplayerStatus(bool showSprite)
{
    if (!showSprite && mpShowingStatus)
    {
        NF_DeleteSprite(1, MP_STATUS_SPRITE);
    }
    else if (showSprite)
    {
        if (!mpShowingStatus && mpCurrentStatus > -1)
        {
            NF_CreateSprite(1, MP_STATUS_SPRITE, TEXT_GFX_ID, TEXT_GFX_ID, (SCREEN_WIDTH / 2) - 32, (SCREEN_HEIGHT / 2) - 16);
        }

        int statusSpriteId;
        switch (mpCurrentStatus)
        {
        case 0:
            statusSpriteId = 12;
            break;
        case 1:
            statusSpriteId = 11;
            break;
        case 2:
            statusSpriteId = 14;
            break;
        case 3:
            statusSpriteId = 13;
            break;
        default:
            statusSpriteId = 15;
            break;
        }

        NF_SpriteFrame(1, MP_STATUS_SPRITE, statusSpriteId);
    }
    mpShowingStatus = showSprite;
}

void Menu::ShowMinerals(bool showSprites, SaveFile *saveFile)
{
    int row = 0;
    int col = 0;
    for (int i = 0; i < MINERAL_COUNT; i++)
    {
        int spriteId = MINERAL_SPRITE_BASE + i;
        row = i / MINERALS_PER_ROW;
        col = i % MINERALS_PER_ROW;
        int pos[2] = {MINERAL_GRID_BASE_POS[0] + ((32 + MINERAL_GAP) * col), MINERAL_GRID_BASE_POS[1] + ((32 + MINERAL_GAP) * row)};

        if (showSprites)
        {
            NF_CreateSprite(1, spriteId, 0, 0, pos[0], pos[1]);
            NF_SpriteFrame(1, spriteId, 4);
            NF_ShowSprite(1, spriteId, !saveFile->minerals[i]);
        }
        else
        {
            NF_DeleteSprite(1, MINERAL_SPRITE_BASE + i);
        }
    }

    if (showSprites)
    {
        NF_CreateSprite(1, MINERAL_CURSOR_SPRITE, 0, 0, MINERAL_GRID_BASE_POS[0], MINERAL_GRID_BASE_POS[1]);
        NF_SpriteFrame(1, MINERAL_CURSOR_SPRITE, 3);

        NF_CreateSprite(1, HANK_EYES_SPRITE, 0, 0, HANK_EYES_POS[0], HANK_EYES_POS[1]);
        NF_SpriteFrame(1, HANK_EYES_SPRITE, 5);
        NF_ShowSprite(1, HANK_EYES_SPRITE, false);
    }
    else
    {
        NF_DeleteSprite(1, MINERAL_CURSOR_SPRITE);
        NF_DeleteSprite(1, HANK_EYES_SPRITE);
    }
}

void Menu::UpdateMinerals(volatile int frame, Sound *sound, SaveFile *saveFile)
{
    int row = 0;
    int col = 0;
    NF_ShowSprite(1, MINERAL_CURSOR_SPRITE, currentlySelectedMineral != -1);

    bool touching = false;
    touchPosition touch;
    if (keysDown() & KEY_TOUCH)
    {
        touchRead(&touch);
        touching = touch.px >= MINERAL_GRID_BASE_POS[0] && touch.px <= MINERAL_GRID_BASE_POS[0] + ((32 + MINERAL_GAP) * MINERALS_PER_ROW) 
            && touch.py > MINERAL_GRID_BASE_POS[1] && touch.py < MINERAL_GRID_BASE_POS[1] + ((32 + MINERAL_GAP) * (MINERAL_COUNT / MINERALS_PER_ROW));

        if (!touching)
        {
            currentlySelectedMineral = -1;
        }
    }

    for (int i = 0; i < MINERAL_COUNT; i++)
    {
        int spriteId = MINERAL_SPRITE_BASE + i;
        NF_ShowSprite(1, spriteId, !saveFile->minerals[i]);

        row = i / MINERALS_PER_ROW;
        col = i % MINERALS_PER_ROW;
        int pos[2] = {MINERAL_GRID_BASE_POS[0] + ((32 + MINERAL_GAP) * col), MINERAL_GRID_BASE_POS[1] + ((32 + MINERAL_GAP) * row)};

        if (touching)
        {
            sound->PlaySFX(SFX_MENU_SELECT);

            bool touchingMineral = touch.px >= pos[0] && touch.px <= pos[0] + 32 && touch.py >= pos[1] && touch.py <= pos[1] + 32;
            currentlySelectedMineral = touchingMineral ? i : currentlySelectedMineral;
        }

        if (currentlySelectedMineral == i)
        {
            NF_MoveSprite(1, MINERAL_CURSOR_SPRITE, pos[0], pos[1]);
        }
    }

    // Hank blink animation
    int blinkRange = (hankLastBlink - frame);
    bool isBlinking = false;
    if (blinkRange < 10 && blinkRange > 0)
    {
        isBlinking = true;
    }
    if (blinkRange <= 0)
    {
        hankLastBlink = frame + (rand() % 100) + 100;
    }
    NF_ShowSprite(1, HANK_EYES_SPRITE, isBlinking);
}

void Menu::Update(volatile int frame, Sound *sound, SaveFile *saveFile)
{
    NF_ClearTextLayer(1, 0);

    switch (state)
    {
    case MENU_LOADING:
        NF_ShowSprite(1, START_SPRITE, false);
        break;

    case MENU_LOGO:
        if (x < TARGET_X)
        {
            x += X_SPEED;
            NE_ModelTranslate(logo, X_SPEED, 0, 0);
        }
        if (textX < TARGET_TEXT_X)
        {
            textX += X_SPEED;
            NE_ModelTranslate(text, X_SPEED, 0, 0);
        }

        NF_SpriteFrame(1, START_SPRITE, (frame > 345) ? 1 : 0);
        NE_ModelRotate(skybox, (frame % 4 == 0 ? 1 : 0), 0, 0);

        if (frame >= 690)
        {
            SetState(MENU_TITLE, frame, sound, saveFile);
            return;
        }
        break;

    case MENU_TITLE:
        if (frame % 30 == 0)
        {
            showStartSprite = !showStartSprite;
        }
        NF_WriteText(1, 0, 26, 0, "v1.0.3");
        break;

    case MENU_RUMBLE:
        if (!isDSiMode()) {
            NF_WriteText(1, 0, 2, 7, "A DS Rumble Pak is required.");
            NF_WriteText(1, 0, 2, 8, "to enable this feature. If a");
            NF_WriteText(1, 0, 2, 9, "Rumble Pak is inserted into");
            NF_WriteText(1, 0, 2, 10, "SLOT-2, you should now feel.");
            NF_WriteText(1, 0, 2, 11, "the rumble effect.");

            NF_WriteText(1, 0, 2, 14, "Remove the Rumble Pak from");
            NF_WriteText(1, 0, 2, 15, "SLOT-2 to disable rumble.");
        
            rumble(frame % 2 == 0);
        } 
        else {
            NF_WriteText(1, 0, 2, 9, "Rumble support is disabled.");
            NF_WriteText(1, 0, 2, 11, "You do not have a system");
            NF_WriteText(1, 0, 2, 12, "that supports the rumble");
            NF_WriteText(1, 0, 2, 13, "feature of this game.");
        }
        break;

    case MENU_MINERALS:
        if (currentlySelectedMineral == -1)
        {
            NF_WriteText(1, 0, 2, 1, "Walt! If you happen upon any");
            NF_WriteText(1, 0, 2, 3, "minerals, I'll appraise 'em!");
        }
        else
        {
            const Mineral *mineral = &MINERALS[currentlySelectedMineral];
            if (saveFile->minerals[currentlySelectedMineral])
            {
                NF_WriteText(1, 0, 1, 1, mineral->name);
                NF_WriteText(1, 0, 1, 3, mineral->description);
            }
            else
            {
                NF_WriteText(1, 0, 1, 1, "???");
                if (mineral->isSecret)
                {
                    NF_WriteText(1, 0, 1, 3, "?????????");
                }
                else
                {
                    NF_WriteText(1, 0, 1, 3, mineral->description);
                }
            }
        }
        UpdateMinerals(frame, sound, saveFile);
        break;

    case MENU_MUSIC_PLAYER:
        NF_WriteText(1, 0, 2, 9, "Currently playing:");
        char trackNoAndTitle[50];
        sprintf(trackNoAndTitle, "%d. %s", currentSoundTestTrack + 1, BGMS[currentSoundTestTrack].name);
        NF_WriteText(1, 0, 2, 10, trackNoAndTitle);
        NF_WriteText(1, 0, 2, 11, sound->GetBgmTrackProgressString());
        NF_WriteText(1, 0, 2, 13, "LEFT/RIGHT to change track.");
        break;

    case MENU_MP_HOST_ROOM:
    case MENU_MP_JOIN_ROOM:
        UpdateMultiplayer();
        break;
    }

    if (GetLayoutForState(state) != nullptr)
    {
        UpdateLayout(frame);
        NF_ShowSprite(1, START_SPRITE, state == MENU_TITLE && showStartSprite);
        NF_SpriteFrame(1, START_SPRITE, 2);
        NE_ModelRotate(skybox, 0, (frame % 8 == 0 ? 1 : 0), 0);
    }
}

MenuSelection Menu::HandleInput(volatile int frame, Sound *sound, SaveFile *saveFile)
{
    touchPosition touch;
    switch (state)
    {
    case MENU_LOGO:
        if (keysDown() & KEY_TOUCH || keysDown() & KEY_A || keysDown() & KEY_START)
        {
            return SKIP_LOGO;
        }
        return NONE;

    case MENU_TITLE:
        if (keysDown() & KEY_TOUCH || keysDown() & KEY_A || keysDown() & KEY_START)
        {
            SetState(MENU_MAIN, frame, sound, saveFile);
        }
        return NONE;

    case MENU_MUSIC_PLAYER:
        if (keysDown() & KEY_RIGHT || keysDown() & KEY_LEFT)
        {
            if (keysDown() & KEY_RIGHT)
            {
                currentSoundTestTrack++;
                sound->PlaySFX(SFX_MENU_SELECT);

                if (currentSoundTestTrack >= BGM_COUNT)
                {
                    currentSoundTestTrack = 0;
                }
            }
            else if (keysDown() & KEY_LEFT)
            {
                currentSoundTestTrack--;
                sound->PlaySFX(SFX_MENU_SELECT);

                if (currentSoundTestTrack < 0)
                {
                    currentSoundTestTrack = BGM_COUNT - 1;
                }
            }
            
            sound->PlayBGM(static_cast<TrackId>(currentSoundTestTrack), true);
            rumble(currentSoundTestTrack % 2 == 0);
        }
        return HandleLayoutInput(frame, sound, saveFile, touch);

    case MENU_MP_HOST_ROOM:
        if (mpCurrentStatus == MP_HOST_READY)
        {
            if (keysDown() & KEY_A)
            {
                sound->PlaySFX(SFX_MENU_DRUM);
                return START_MP_GAME;
            }
            break;
        }
        break;
    case MENU_MP_JOIN_ROOM:
        if (mpCurrentStatus == MP_CLIENT_READY && (getOpponent()->timeLeft > -1))
        {
            sound->PlaySFX(SFX_MENU_DRUM);
            return START_MP_GAME;
        }
        break;
    }

    return HandleLayoutInput(frame, sound, saveFile, touch);
}

MenuSelection Menu::HandleLayoutInput(volatile int frame, Sound *sound, SaveFile *saveFile, touchPosition touch)
{
    const Layout *layout = GetLayoutForState(state);
    if (layout == nullptr)
    {
        return NONE;
    }

    if (keysDown() & KEY_TOUCH)
    {
        touchRead(&touch);

        for (int b = 0; b < layout->buttonCount; b++)
        {
            if (IsTouchInBox(layout->buttonCoords[b], layout->buttons[b].dimensions, touch))
            {
                return HandleClick(layout->buttons[b].selection, frame, sound, saveFile);
            }
        }
        highlightedItem = NONE;
        return NONE;
    }

    if (keysDown() & KEY_B)
    {   
        SetState(layout->backState, frame, sound, saveFile);
        return NONE;
    }

    if (keysDown() & KEY_A || keysDown() & KEY_START)
    {
        return HandleClick(highlightedItem, frame, sound, saveFile);
    }

    if (!layout->allowCaretNavigation)
    {
        return NONE;
    }

    if (keysDown() & KEY_UP || keysDown() & KEY_DOWN || keysDown() & KEY_LEFT || keysDown() & KEY_RIGHT)
    {
        int dCaret[2] = {0, 0};
        if (keysDown() & KEY_UP)
        {
            dCaret[0]--;
        }
        else if (keysDown() & KEY_DOWN)
        {
            dCaret[0]++;
        }
        else if (keysDown() & KEY_LEFT)
        {
            dCaret[1]--;
        }
        else if (keysDown() & KEY_RIGHT)
        {
            dCaret[1]++;
        }

        if (menuCaret[0] + dCaret[0] < 0)
        {
            dCaret[0] = 2;
        }
        else if (menuCaret[0] + dCaret[0] > 2)
        {
            dCaret[0] = 0;
        }
        if (menuCaret[1] + dCaret[1] < 0)
        {
            dCaret[1] = 1;
        }
        else if (menuCaret[1] + dCaret[1] > 1)
        {
            dCaret[1] = 0;
        }

        MenuSelection selection = layout->caretMap[menuCaret[0] + dCaret[0]][menuCaret[1] + dCaret[1]];
        if (selection != NONE)
        {
            menuCaret[0] += dCaret[0];
            menuCaret[1] += dCaret[1];
            highlightedItem = selection;
            sound->PlaySFX(SFX_MENU_SELECT);
        }
    }

    return NONE;
}

MenuSelection Menu::HandleClick(MenuSelection clicked, volatile int frame, Sound *sound, SaveFile *saveFile)
{
    switch (clicked)
    {
    case OPEN_GAME_MENU:
        if (CheckSelection(clicked))
        {
            SetState(MENU_GAME_SELECT, frame, sound, saveFile);
            return OPEN_GAME_MENU;
        }
        return NONE;
    case TOGGLE_RUMBLE:
        if (CheckSelection(clicked))
        {
            SetState(MENU_RUMBLE, frame, sound, saveFile);
            return TOGGLE_RUMBLE;
        }
        return NONE;
    case BACK_TO_TITLE:
        if (CheckSelection(clicked))
        {
            SetState(MENU_TITLE, frame, sound, saveFile);
            return BACK_TO_TITLE;
        }
        return NONE;
    case BACK_TO_MAIN_MENU:
        if (CheckSelection(clicked))
        {
            SetState(MENU_MAIN, frame, sound, saveFile);
            return BACK_TO_MAIN_MENU;
        }
        return NONE;
    case OPEN_ROOM:
        if (CheckSelection(clicked))
        {
            SetState(MENU_MP_HOST_ROOM, frame, sound, saveFile);
            return OPEN_ROOM;
        }
        return NONE;
    case SEARCH_FOR_ROOMS:
        if (CheckSelection(clicked))
        {
            SetState(MENU_MP_JOIN_ROOM, frame, sound, saveFile);
            return SEARCH_FOR_ROOMS;
        }
        return NONE;
    case OPEN_EXTRAS_MENU:
        if (CheckSelection(clicked))
        {
            SetState(MENU_EXTRAS, frame, sound, saveFile);
            return OPEN_EXTRAS_MENU;
        }
        return NONE;
    case OPEN_SOUND_TEST_MENU:
        if (CheckSelection(clicked))
        {
            SetState(MENU_MUSIC_PLAYER, frame, sound, saveFile);
            return OPEN_SOUND_TEST_MENU;
        }
        return NONE;
    case OPEN_MINERALS_MENU:
        if (CheckSelection(clicked))
        {
            SetState(MENU_MINERALS, frame, sound, saveFile);
            return OPEN_MINERALS_MENU;
        }
        return NONE;
    case BACK_TO_GAME_MENU:
        if (CheckSelection(clicked))
        {
            if (state == MENU_MP_HOST_ROOM || state == MENU_MP_JOIN_ROOM)
            {
                if (mpCurrentStatus == MP_HOST_READY || mpCurrentStatus == MP_CLIENT_READY)
                {
                    rumble(rand() % 2 == 0);
                    return NONE;
                }
                SetState(MENU_GAME_SELECT, frame, sound, saveFile);
                mpCurrentStatus = -1;
                mpCreatingRoom = false;
                disableMultiplayer();
                return BACK_TO_GAME_MENU;
            }
            SetState(MENU_GAME_SELECT, frame, sound, saveFile);
            return BACK_TO_GAME_MENU;
        }
        return NONE;
    case BACK_TO_EXTRAS_MENU:
        if (CheckSelection(clicked))
        {
            SetState(MENU_EXTRAS, frame, sound, saveFile);
            return BACK_TO_EXTRAS_MENU;
        }
        return NONE;
    default:
        if (CheckSelection(clicked))
        {
            return clicked;
        }
        return NONE;
    }
    return NONE;
}

bool Menu::IsTouchInBox(const int coords[2], const int boxDimensions[2], touchPosition touch)
{
    int xOffset = (128 - boxDimensions[0]) / 2;
    int yOffset = (64 - boxDimensions[1]) / 2;
    int boxPos[2] = {coords[0] + xOffset, coords[1] + yOffset};

    return (touch.px >= boxPos[0] && touch.px <= boxPos[0] + boxDimensions[0] &&
            touch.py >= boxPos[1] && touch.py <= boxPos[1] + boxDimensions[1]);
}

MenuSelection Menu::CheckSelection(MenuSelection tappedBox)
{
    if (tappedBox == highlightedItem)
    {
        return highlightedItem;
    }
    highlightedItem = tappedBox;
    return NONE;
}

void Menu::Draw(volatile int frame)
{
    if (state != MENU_LOADING)
    {
        NE_ModelDraw(skybox);
        NE_PolyFormat(31, 8, NE_LIGHT_0, NE_CULL_NONE, NE_FOG_ENABLE);
        NE_ModelDraw(logo);
        NE_ModelDraw(text);
    }
}

void Menu::Unload(volatile int frame, Sound *sound, SaveFile *saveFile)
{
    SetState(MENU_LOADING, frame, sound, saveFile);
    ShowLayout();
    ShowBackground();
    ShowMultiplayerStatus(false);

    // Free the model and material
    NE_ModelDelete(logo);
    NE_ModelDelete(text);
    NE_ModelDelete(skybox);

    NE_MaterialDelete(logoMaterial);
    NE_MaterialDelete(textMaterial);
    NE_MaterialDelete(skyboxMaterial);

    // Unload sprites
    UnloadInterfaceSprites();
    NF_VramSpriteGfxDefrag(1);
}