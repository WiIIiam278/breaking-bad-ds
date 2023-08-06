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

    // Sound test sequence
    currentSequenceIndex = 0;

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
                              RGB15(0, 0, 0), // Diffuse
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

    // Prepare sprites
    LoadSplashSprite();

    NF_LoadSpriteGfx("sprite/main_menu", 2, 64, 32);
    NF_LoadSpritePal("sprite/main_menu", 2);
    NF_VramSpriteGfx(1, 2, 1, false);
    NF_VramSpritePal(1, 2, 1);
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

void Menu::ShowLayout()
{
    const Layout *layout;
    if (buttonsVisible)
    {
        for (int i = 0; i < BUTTON_TEXT_COUNT; i++)
        {
            NF_DeleteSprite(1, BUTTON_TEXT[i]);
        }
        for (int i = 0; i < BUTTON_SPRITE_COUNT; i++)
        {
            NF_DeleteSprite(1, BUTTON_SPRITES[i]);
        }
        NF_DeleteSprite(1, TITLE_SPRITE);
    }
    switch (state)
    {
    default:
        buttonsVisible = false;
        break;
    case MENU_MAIN:
    case MENU_GAME_SELECT:
        layout = &LAYOUTS[state == MENU_MAIN ? 0 : 1];
        menuCaret[0] = menuCaret[1] = 0;
        highlightedItem = layout->caretMap[0][0];

        for (int b = 0; b < 4; b++)
        {
            int spriteOffset = b * 4;
            Button button = layout->buttons[b];
            for (int i = 0; i < 4; i++)
            {
                int xOffset = (i == 1 || i == 3) ? 64 : 0;
                int yOffset = (i == 2 || i == 3) ? 32 : 0;
                int spriteId = BUTTON_SPRITES[spriteOffset + i];
                NF_CreateSprite(1, spriteId, 1, 1, layout->buttonCoords[b][0] + xOffset, layout->buttonCoords[b][1] + yOffset);
                NF_HflipSprite(1, spriteId, i % 2 != 0);
                NF_VflipSprite(1, spriteId, i > 1);
                NF_SpriteFrame(1, spriteId, button.buttonSprite);
            }
            NF_CreateSprite(1, BUTTON_TEXT[b], 1, 1, layout->buttonCoords[b][0] + 32, layout->buttonCoords[b][1] + 16);
            NF_SpriteFrame(1, BUTTON_TEXT[b], button.textSprite);
        }
        NF_CreateSprite(1, TITLE_SPRITE, 1, 1, layout->titleCoords[0], layout->titleCoords[1]);
        NF_SpriteFrame(1, TITLE_SPRITE, layout->titleSprite);

        buttonsVisible = true;
        break;
    }
}

void Menu::UpdateLayout(volatile int frame)
{
    if (!(state == MENU_MAIN || state == MENU_GAME_SELECT))
    {
        return;
    }
    const Layout *layout = &LAYOUTS[state == MENU_MAIN ? 0 : 1];
    for (int b = 0; b < 4; b++)
    {
        int spriteOffset = b * 4;
        Button button = layout->buttons[b];
        for (int i = 0; i < 4; i++)
        {
            int spriteId = BUTTON_SPRITES[spriteOffset + i];
            NF_SpriteFrame(1, spriteId, button.buttonSprite + (highlightedItem == (b + (state == MENU_MAIN ? 1 : 5)) ? 4 : 0));
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
    ShowLayout();
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
        NE_ModelRotate(skybox, (frame % 4 == 0 ? 1 : 0), 0, 0);

        if (frame >= 690)
        {
            SetState(MENU_TITLE, sound);
            return;
        }
        break;

    case MENU_TITLE:
        NF_SpriteFrame(1, START_SPRITE, 2);
        NE_ModelRotate(skybox, 0, (frame % 6 == 0 ? 1 : 0), 0);

        if (frame % 30 == 0)
        {
            showStartSprite = !showStartSprite;
            NF_ShowSprite(1, START_SPRITE, showStartSprite);
        }
        break;

    case MENU_MAIN:
    case MENU_GAME_SELECT:
        UpdateLayout(frame);
        NF_ShowSprite(1, START_SPRITE, false);
        NE_ModelRotate(skybox, 0, (frame % 8 == 0 ? 1 : 0), 0);
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
    case MENU_GAME_SELECT:
        if (keysDown() & KEY_TOUCH)
        {
            const Layout *layout = &LAYOUTS[state == MENU_MAIN ? 0 : 1];
            touchRead(&touch);

            for (int b = 0; b < 4; b++)
            {
                if (IsTouchInBox(layout->buttonCoords[b], layout->buttons[b].dimensions, touch))
                {
                    return HandleClick(layout->buttons[b].selection, sound);
                }
            }
            highlightedItem = NONE;
            return NONE;
        }

        if (keysDown() & KEY_B)
        {
            const bool quitToTitle = state == MENU_MAIN;
            SetState(quitToTitle ? MENU_TITLE : MENU_MAIN, sound);
            return quitToTitle ? BACK_TO_TITLE : BACK_TO_MAIN_MENU;
        }

        if (state == MENU_MAIN && (keysDown() & SOUND_TEST_SEQUENCE[currentSequenceIndex]))
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

        if (keysDown() & KEY_A || keysDown() & KEY_START)
        {
            return HandleClick(highlightedItem, sound);
        }

        if (keysDown() & KEY_UP || keysDown() & KEY_DOWN || keysDown() & KEY_LEFT || keysDown() & KEY_RIGHT)
        {
            const Layout *layout = &LAYOUTS[state == MENU_MAIN ? 0 : 1];
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

MenuSelection Menu::HandleClick(MenuSelection clicked, Sound *sound)
{
    switch (clicked)
    {
    case OPEN_GAME_MENU:
        if (CheckSelection(clicked))
        {
            SetState(MENU_GAME_SELECT, sound);
            return OPEN_GAME_MENU;
        }
        return NONE;
    case TOGGLE_RUMBLE:
        if (CheckSelection(clicked))
        {
            SetState(MENU_RUMBLE, sound);
            return TOGGLE_RUMBLE;
        }
        return NONE;
    case BACK_TO_TITLE:
        if (CheckSelection(clicked))
        {
            SetState(MENU_TITLE, sound);
            return BACK_TO_TITLE;
        }
        return NONE;
    case BACK_TO_MAIN_MENU:
        if (CheckSelection(clicked))
        {
            SetState(MENU_MAIN, sound);
            return BACK_TO_MAIN_MENU;
        }
        return NONE;
    case OPEN_ROOM:
        if (CheckSelection(clicked))
        {
            SetState(MENU_MP_HOST_ROOM, sound);
            return OPEN_ROOM;
        }
        return NONE;
    case SEARCH_FOR_ROOMS:
        if (CheckSelection(clicked))
        {
            SetState(MENU_MP_JOIN_ROOM, sound);
            return SEARCH_FOR_ROOMS;
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

void Menu::Unload(Sound *sound)
{
    SetState(MENU_LOADING, sound);
    ShowLayout();
    ShowBackground();

    // Free the model and material
    NE_ModelDelete(logo);
    NE_ModelDelete(text);
    NE_ModelDelete(skybox);

    NE_MaterialDelete(logoMaterial);
    NE_MaterialDelete(textMaterial);
    NE_MaterialDelete(skyboxMaterial);

    // Delete sprite
    ShowMultiplayerStatus(false);
    NF_DeleteSprite(1, START_SPRITE);
    NF_UnloadSpriteGfx(1);
    NF_UnloadSpritePal(1);
    NF_FreeSpriteGfx(1, 0);
    NF_UnloadSpriteGfx(2);
    NF_UnloadSpritePal(2);
    NF_FreeSpriteGfx(1, 1);
}