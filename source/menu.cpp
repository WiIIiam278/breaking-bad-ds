#include "menu.h"

Menu::Menu()
{
    state = MENU_LOADING;
    highlightedItem = NONE;
}

int Menu::Load()
{
    backdrop = NE_ModelCreate(NE_Static);
    backdropMaterial = NE_MaterialCreate();

    logo = NE_ModelCreate(NE_Static);
    logoMaterial = NE_MaterialCreate();

    text = NE_ModelCreate(NE_Static);
    textMaterial = NE_MaterialCreate();

    // Load assets from the filesystem
    if (NE_ModelLoadStaticMeshFAT(backdrop, "model/backdrop.dl") == 0 || NE_ModelLoadStaticMeshFAT(logo, "model/logo.dl") == 0 || NE_ModelLoadStaticMeshFAT(text, "model/logo_text.dl") == 0)
    {
        consoleDemoInit();
        printf("Couldn't load logo mesh/text plane...");
        return -1;
    }
    if (NE_MaterialTexLoadFAT(backdropMaterial, NE_A1RGB5, 128, 128, NE_TEXGEN_TEXCOORD, "model/backdrop_tex.bin") == 0 || NE_MaterialTexLoadFAT(logoMaterial, NE_A1RGB5, 128, 128, NE_TEXGEN_TEXCOORD, "model/logo_tex.bin") == 0 || NE_MaterialTexLoadFAT(textMaterial, NE_A1RGB5, 128, 128, NE_TEXGEN_TEXCOORD, "model/logo_text_tex.bin") == 0)
    {
        consoleDemoInit();
        printf("Couldn't load logo/text textures...");
        return -1;
    }

    // Sound test sequence
    currentSequenceIndex = 0;

    // Assign material to the model
    NE_ModelSetMaterial(backdrop, backdropMaterial);
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

    // Set model rotation and scale
    int scale = 8250;
    NE_ModelScaleI(backdrop, scale * 3, scale * 3, scale * 3);
    NE_ModelScaleI(logo, scale, scale, scale);
    NE_ModelScaleI(text, scale, scale, scale);

    // Position models
    x = TARGET_X - 10;
    textX = TARGET_X - 16;
    NE_ModelSetCoordI(backdrop, 0, 0, 0);
    NE_ModelSetCoordI(logo, 0, 0, 0);
    NE_ModelSetCoordI(text, 0, 0, 0);
    NE_ModelTranslate(backdrop, x + 15, Y - 10, Z + 5);
    NE_ModelTranslate(logo, x, Y, Z);
    NE_ModelTranslate(text, textX, Y + 0.1, Z + 4.55);

    // Prepare sprites
    LoadSplashSprite();
    return 0;
}

void Menu::LoadSplashSprite()
{
    // Load credits splash / touch to start sprite
    NF_LoadSpriteGfx("sprite/start", 1, 64, 64);
    NF_LoadSpritePal("sprite/start", 1);
    NF_VramSpriteGfx(1, 1, 0, false);
    NF_VramSpritePal(1, 1, 0);

    // Create, position & scale sprite
    NF_CreateSprite(1, START_SPRITE, 0, 0, 64, 32);
    NF_EnableSpriteRotScale(1, START_SPRITE, START_SPRITE, true);
    NF_SpriteRotScale(1, START_SPRITE, 0, 283, 283);
}

void Menu::ShowButtons()
{
    if (buttonsVisible)
    {
        for (int i = 0; i < 4; i++)
        {
            NF_DeleteSprite(1, BUTTONS[i]);
        }
        NF_UnloadSpriteGfx(2);
        NF_UnloadSpritePal(2);
        NF_FreeSpriteGfx(1, 1);
    }
    switch (state)
    {
    default:
        buttonsVisible = false;
        break;
    case MENU_MAIN:
        highlightedItem = NONE;
        NF_LoadSpriteGfx("sprite/buttons", 2, 64, 64);
        NF_LoadSpritePal("sprite/buttons", 2);
        NF_VramSpriteGfx(1, 2, 1, false);
        NF_VramSpritePal(1, 2, 1);
        for (int i = 0; i < 4; i++)
        {
            // Create, position & scale sprite
            NF_CreateSprite(1, BUTTONS[i], 1, 1, MAIN_BUTTON_COORDS[i][0], MAIN_BUTTON_COORDS[i][1]);
            NF_EnableSpriteRotScale(1, BUTTONS[i], BUTTONS[i], true);
            NF_SpriteRotScale(1, BUTTONS[i], 0, 300, 300);
            NF_SpriteFrame(1, BUTTONS[i], i * 2);
        }
        buttonsVisible = true;
        break;
    case MENU_GAME_SELECT:
        highlightedItem = NONE;
        NF_LoadSpriteGfx("sprite/game_buttons", 2, 64, 64);
        NF_LoadSpritePal("sprite/game_buttons", 2);
        NF_VramSpriteGfx(1, 2, 1, false);
        NF_VramSpritePal(1, 2, 1);
        for (int i = 0; i < 4; i++)
        {
            // Create, position & scale sprite
            NF_CreateSprite(1, BUTTONS[i], 1, 1, GAME_BUTTON_COORDS[i][0], GAME_BUTTON_COORDS[i][1]);
            NF_EnableSpriteRotScale(1, BUTTONS[i], BUTTONS[i], true);
            NF_SpriteRotScale(1, BUTTONS[i], 0, 300, 300);
            NF_SpriteFrame(1, BUTTONS[i], i * 2);
        }
        buttonsVisible = true;
        break;
    }
}

void Menu::ShowBackground()
{
    if ((state == MENU_LOGO || state == MENU_LOADING))
    {
        if (currentBackground != 0)
        {
            NF_DeleteTiledBg(1, MENU_BG_ID);
            NF_UnloadTiledBg(currentBackground == 1 ? TITLE_BG_NAME : MP_BG_NAME);
        }
        currentBackground = 0;
    }
    else if ((state == MENU_MP_HOST_ROOM || state == MENU_MP_JOIN_ROOM) && currentBackground != 2)
    {
        if (currentBackground == 1)
        {
            NF_DeleteTiledBg(1, MENU_BG_ID);
            NF_UnloadTiledBg(TITLE_BG_NAME);
        }
        NF_LoadTiledBg(MP_BG_NAME, MP_BG_NAME, 256, 256);
        NF_CreateTiledBg(1, MENU_BG_ID, MP_BG_NAME);
        currentBackground = 2;
    }
    else if (currentBackground != 1)
    {
        if (currentBackground == 2)
        {
            NF_DeleteTiledBg(1, MENU_BG_ID);
            NF_UnloadTiledBg(MP_BG_NAME);
        }
        NF_LoadTiledBg(TITLE_BG_NAME, TITLE_BG_NAME, 256, 256);
        NF_CreateTiledBg(1, MENU_BG_ID, TITLE_BG_NAME);
        currentBackground = 1;
    }
}

void Menu::SetState(MenuState newState, Sound *sound)
{
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

    if (state == MENU_SOUND_TEST && newState == MENU_MAIN)
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

    if (state != MENU_LOGO)
    {
        sound->PlaySFX(SFX_MENU_DRUM);
        setRumble(state % 2 == 0);
    }
    currentSequenceIndex = 0;
    state = newState;
    ShowButtons();
    ShowBackground();
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
        NF_UnloadSpriteGfx(MP_STATUS_SPRITE);
        NF_UnloadSpritePal(MP_STATUS_SPRITE);
        NF_FreeSpriteGfx(1, MP_STATUS_SPRITE);
    }
    else if (showSprite)
    {
        if (!mpShowingStatus && mpCurrentStatus > -1)
        {
            NF_LoadSpriteGfx("sprite/multiplayer_status", MP_STATUS_SPRITE, 64, 64);
            NF_LoadSpritePal("sprite/multiplayer_status", MP_STATUS_SPRITE);
            NF_VramSpriteGfx(1, MP_STATUS_SPRITE, MP_STATUS_SPRITE, false);
            NF_VramSpritePal(1, MP_STATUS_SPRITE, MP_STATUS_SPRITE);
            NF_CreateSprite(1, MP_STATUS_SPRITE, MP_STATUS_SPRITE, MP_STATUS_SPRITE, 64, 42);
            NF_EnableSpriteRotScale(1, MP_STATUS_SPRITE, MP_STATUS_SPRITE, true);
            NF_SpriteRotScale(1, MP_STATUS_SPRITE, 0, 283, 283);
        }
        NF_SpriteFrame(1, MP_STATUS_SPRITE, mpCurrentStatus);
    }
    mpShowingStatus = showSprite;
}

void Menu::Update(volatile int frame, Sound *sound)
{
    NF_ClearTextLayer(1, 0);
    switch (state)
    {
    case MENU_LOADING:
        // Hide buttons and backgrounds
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

        if (frame >= 690)
        {
            SetState(MENU_TITLE, sound);
            return;
        }
        break;

    case MENU_TITLE:
        NF_SpriteFrame(1, START_SPRITE, 2);

        if (frame % 30 == 0)
        {
            showStartSprite = !showStartSprite;
            NF_ShowSprite(1, START_SPRITE, showStartSprite);
        }
        break;

    case MENU_MAIN:
        // Update button highlighting
        for (int i = 0; i < 4; i++)
        {
            NF_SpriteFrame(1, BUTTONS[i], (i * 2) + (((i + 1) == highlightedItem) ? 1 : 0));
        }

        NF_ShowSprite(1, START_SPRITE, false);
        break;

    case MENU_RUMBLE:
        NF_ShowSprite(1, START_SPRITE, false);

        NF_WriteText(1, 0, 1, 6, "A DS Rumble Pak is required.");
        NF_WriteText(1, 0, 1, 7, "to enable this feature. If a");
        NF_WriteText(1, 0, 1, 8, "Rumble Pak is inserted into");
        NF_WriteText(1, 0, 1, 9, "SLOT-2, you should now feel.");
        NF_WriteText(1, 0, 1, 10, "the rumble effect.");
        NF_WriteText(1, 0, 1, 12, "Remove the Rumble Pak from");
        NF_WriteText(1, 0, 1, 13, "SLOT-2 to disable rumble.");
        setRumble(frame % 2 == 0);

        NF_WriteText(1, 0, 1, 15, "Touch the screen to continue.");
        break;

    case MENU_SOUND_TEST:
        NF_ShowSprite(1, START_SPRITE, false);

        NF_WriteText(1, 0, 1, 5, "Breaking Bad DS - SOUND TEST");
        NF_WriteText(1, 0, 1, 6, "LEFT/RIGHT to change track.");
        NF_WriteText(1, 0, 1, 10, "Currently playing:");
        NF_WriteText(1, 0, 1, 11, BGMS[currentSoundTestTrack].name);
        NF_WriteText(1, 0, 1, 12, sound->GetBgmTrackProgressString());

        NF_WriteText(1, 0, 1, 15, "Touch the screen to continue.");
        break;

    case MENU_GAME_SELECT:
        // Show buttons
        for (int i = 0; i < 4; i++)
        {
            NF_SpriteFrame(1, BUTTONS[i], (i * 2) + (((i + 5) == (highlightedItem)) ? 1 : 0));
        }

        NF_ShowSprite(1, START_SPRITE, false);
        break;

    case MENU_MP_HOST_ROOM:
    case MENU_MP_JOIN_ROOM:
        UpdateMultiplayer();
        break;
    }
}

MenuSelection Menu::HandleInput(Sound *sound)
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
            SetState(MENU_MAIN, sound);
        }
        return NONE;
    case MENU_SOUND_TEST:
        if (keysDown() & KEY_RIGHT || keysDown() & KEY_LEFT)
        {
            if (keysDown() & KEY_RIGHT)
            {
                currentSoundTestTrack++;
            }
            else if (keysDown() & KEY_LEFT)
            {
                currentSoundTestTrack--;
            }
            currentSoundTestTrack = (currentSoundTestTrack < 0)                 ? 0
                                    : (currentSoundTestTrack > (BGM_COUNT - 1)) ? (BGM_COUNT - 1)
                                                                                : currentSoundTestTrack;
            sound->PlayBGM(static_cast<TrackId>(currentSoundTestTrack), true);
            setRumble(currentSoundTestTrack % 2 == 0);
        }

    case MENU_RUMBLE:
        if (keysDown() & KEY_TOUCH || keysDown() & KEY_B)
        {
            SetState(MENU_MAIN, sound);
        }
        return NONE;
    case MENU_MAIN:
        if (keysDown() & KEY_TOUCH)
        {
            touchRead(&touch);

            if (IsTouchInBox(MAIN_BUTTON_COORDS[0], 64, touch))
            {
                if (CheckSelection(OPEN_GAME_MENU))
                {
                    SetState(MENU_GAME_SELECT, sound);
                    return OPEN_GAME_MENU;
                }
                return NONE;
            }
            else if (IsTouchInBox(MAIN_BUTTON_COORDS[1], 64, touch))
            {
                return CheckSelection(START_TUTORIAL);
            }
            else if (IsTouchInBox(MAIN_BUTTON_COORDS[2], 32, touch))
            {
                if (CheckSelection(BACK_TO_TITLE))
                {
                    SetState(MENU_TITLE, sound);
                    return BACK_TO_TITLE;
                }
                return NONE;
            }
            else if (IsTouchInBox(MAIN_BUTTON_COORDS[3], 32, touch))
            {
                if (CheckSelection(TOGGLE_RUMBLE))
                {
                    SetState(MENU_RUMBLE, sound);
                    return TOGGLE_RUMBLE;
                }
                return NONE;
            }
            break;
        }

        if (keysDown() & SOUND_TEST_SEQUENCE[currentSequenceIndex])
        {
            currentSequenceIndex++;
            if (currentSequenceIndex == 11)
            {
                SetState(MENU_SOUND_TEST, sound);
                return NONE;
            }
            else if (currentSequenceIndex > 8)
            {
                return NONE;
            }
        }
        if (keysDown() & KEY_UP && !(highlightedItem == OPEN_GAME_MENU || highlightedItem == START_TUTORIAL))
        {
            highlightedItem = (highlightedItem == TOGGLE_RUMBLE ? OPEN_GAME_MENU : START_TUTORIAL);
            sound->PlaySFX(SFX_MENU_SELECT);
        }
        else if (keysDown() & KEY_DOWN && !(highlightedItem == TOGGLE_RUMBLE || highlightedItem == BACK_TO_TITLE))
        {
            highlightedItem = (highlightedItem == OPEN_GAME_MENU ? TOGGLE_RUMBLE : BACK_TO_TITLE);
            sound->PlaySFX(SFX_MENU_SELECT);
        }
        else if (keysDown() & KEY_LEFT && !(highlightedItem == OPEN_GAME_MENU || highlightedItem == TOGGLE_RUMBLE))
        {
            highlightedItem = (highlightedItem == START_TUTORIAL ? OPEN_GAME_MENU : TOGGLE_RUMBLE);
            sound->PlaySFX(SFX_MENU_SELECT);
        }
        else if (keysDown() & KEY_RIGHT && !(highlightedItem == BACK_TO_TITLE || highlightedItem == START_TUTORIAL))
        {
            highlightedItem = (highlightedItem == OPEN_GAME_MENU ? START_TUTORIAL : BACK_TO_TITLE);
            sound->PlaySFX(SFX_MENU_SELECT);
        }

        if (keysDown() & KEY_B)
        {
            SetState(MENU_TITLE, sound);
            return BACK_TO_TITLE;
        }

        if (keysDown() & KEY_A || keysDown() & KEY_START)
        {
            switch (highlightedItem)
            {
            default:
                return CheckSelection(highlightedItem);
            case OPEN_GAME_MENU:
                if (CheckSelection(OPEN_GAME_MENU))
                {
                    SetState(MENU_GAME_SELECT, sound);
                    return OPEN_GAME_MENU;
                }
            case BACK_TO_TITLE:
                if (CheckSelection(BACK_TO_TITLE))
                {
                    SetState(MENU_TITLE, sound);
                    return BACK_TO_TITLE;
                }
                return NONE;
            case TOGGLE_RUMBLE:
                if (CheckSelection(TOGGLE_RUMBLE))
                {
                    SetState(MENU_RUMBLE, sound);
                    return TOGGLE_RUMBLE;
                }
                return NONE;
            }
        }
        break;

    case MENU_GAME_SELECT:
        if (keysDown() & KEY_TOUCH)
        {
            touchRead(&touch);

            if (IsTouchInBox(GAME_BUTTON_COORDS[0], 64, touch))
            {
                return CheckSelection(START_1P_GAME);
            }
            else if (IsTouchInBox(GAME_BUTTON_COORDS[1], 32, touch))
            {
                if (CheckSelection(OPEN_ROOM))
                {
                    SetState(MENU_MP_HOST_ROOM, sound);
                    return OPEN_GAME_MENU;
                }
                return NONE;
            }
            else if (IsTouchInBox(GAME_BUTTON_COORDS[2], 32, touch))
            {
                if (CheckSelection(BACK_TO_MAIN_MENU))
                {
                    SetState(MENU_MAIN, sound);
                    return BACK_TO_MAIN_MENU;
                }
                return NONE;
            }
            else if (IsTouchInBox(GAME_BUTTON_COORDS[3], 32, touch))
            {
                if (CheckSelection(SEARCH_FOR_ROOMS))
                {
                    SetState(MENU_MP_JOIN_ROOM, sound);
                    return SEARCH_FOR_ROOMS;
                }
                return NONE;
            }
            break;
        }

        if (keysDown() & KEY_UP)
        {
            if (highlightedItem == SEARCH_FOR_ROOMS)
            {
                highlightedItem = OPEN_ROOM;
            }
            else
            {
                highlightedItem = SEARCH_FOR_ROOMS;
            }
            sound->PlaySFX(SFX_MENU_SELECT);
        }
        else if (keysDown() & KEY_DOWN)
        {
            if (highlightedItem == OPEN_ROOM)
            {
                highlightedItem = SEARCH_FOR_ROOMS;
            }
            else
            {
                highlightedItem = BACK_TO_MAIN_MENU;
            }
            sound->PlaySFX(SFX_MENU_SELECT);
        }
        else if ((keysDown() & KEY_LEFT) && (highlightedItem == OPEN_ROOM || highlightedItem == SEARCH_FOR_ROOMS))
        {
            highlightedItem = START_1P_GAME;
            sound->PlaySFX(SFX_MENU_SELECT);
        }
        else if ((keysDown() & KEY_RIGHT) && (highlightedItem == START_1P_GAME))
        {
            highlightedItem = OPEN_ROOM;
            sound->PlaySFX(SFX_MENU_SELECT);
        }

        if (keysDown() & KEY_B)
        {
            SetState(MENU_MAIN, sound);
            return NONE;
        }

        if (keysDown() & KEY_A || keysDown() & KEY_START)
        {
            switch (highlightedItem)
            {
            default:
                return CheckSelection(highlightedItem);
            case BACK_TO_MAIN_MENU:
                if (CheckSelection(BACK_TO_MAIN_MENU))
                {
                    SetState(MENU_MAIN, sound);
                }
                return NONE;
            case OPEN_ROOM:
                if (CheckSelection(OPEN_ROOM))
                {
                    SetState(MENU_MP_HOST_ROOM, sound);
                    return TOGGLE_RUMBLE;
                }
                return NONE;
            case SEARCH_FOR_ROOMS:
                if (CheckSelection(SEARCH_FOR_ROOMS))
                {
                    SetState(MENU_MP_JOIN_ROOM, sound);
                    return TOGGLE_RUMBLE;
                }
                return NONE;
            }
        }
        break;

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
        if (keysDown() & KEY_B)
        {
            if (mpCurrentStatus == MP_HOST_READY || mpCurrentStatus == MP_CLIENT_READY)
            {
                setRumble(touch.px % 2 == 0);
                break;
            }
            SetState(MENU_GAME_SELECT, sound);
            mpCurrentStatus = -1;
            mpCreatingRoom = false;
            disableMultiplayer();
            sound->PlaySFX(SFX_MENU_DRUM);
            return BACK_TO_GAME_MENU;
        }
        break;
    case MENU_MP_JOIN_ROOM:
        if (mpCurrentStatus == MP_CLIENT_READY && (getOpponent()->timeLeft > -1))
        {
            sound->PlaySFX(SFX_MENU_DRUM);
            return START_MP_GAME;
        }
        if (keysDown() & KEY_B)
        {
            if (mpCurrentStatus == MP_HOST_READY || mpCurrentStatus == MP_CLIENT_READY)
            {
                setRumble(touch.px % 2 == 0);
                break;
            }
            SetState(MENU_GAME_SELECT, sound);
            mpCurrentStatus = -1;
            disableMultiplayer();
            sound->PlaySFX(SFX_MENU_DRUM);
            return BACK_TO_GAME_MENU;
        }
        break;
    }

    return NONE;
}

bool Menu::IsTouchInBox(const float coords[2], int boxHeight, touchPosition touch)
{
    // Adjust the touch coordinates to account for the sprite scaling
    const float scale = 2.1f;
    float scaledTouchX = touch.px / scale;
    float scaledTouchY = touch.py / scale;

    // Calculate half width and height of the box
    float halfBoxWidth = 32.0f;
    float halfBoxHeight = static_cast<float>(boxHeight) / 2.0f;

    // Calculate the boundaries of the box
    float leftBoundary = coords[0] - halfBoxWidth;
    float rightBoundary = coords[0] + halfBoxWidth;
    float topBoundary = coords[1] - halfBoxHeight;
    float bottomBoundary = coords[1] + halfBoxHeight;

    // Check if the adjusted touch coordinates are within the box boundaries
    if (scaledTouchX >= leftBoundary && scaledTouchX <= rightBoundary &&
        scaledTouchY >= topBoundary && scaledTouchY <= bottomBoundary)
    {
        return true; // Touch is inside the box
    }

    return false; // Touch is outside the box
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
        NE_ModelDraw(backdrop);
        NE_PolyFormat(31, 8, NE_LIGHT_0, NE_CULL_NONE, NE_FOG_ENABLE);
        NE_ModelDraw(logo);
        NE_ModelDraw(text);
    }
}

void Menu::Unload(Sound *sound)
{
    SetState(MENU_LOADING, sound);
    ShowButtons();
    ShowBackground();

    // Free the model and material
    NE_ModelDelete(logo);
    NE_ModelDelete(text);
    NE_ModelDelete(backdrop);

    NE_MaterialDelete(logoMaterial);
    NE_MaterialDelete(textMaterial);
    NE_MaterialDelete(backdropMaterial);

    // Delete sprite
    ShowMultiplayerStatus(false);
    NF_DeleteSprite(1, START_SPRITE);
    NF_UnloadSpriteGfx(1);
    NF_UnloadSpritePal(1);
    NF_FreeSpriteGfx(1, 0);
}