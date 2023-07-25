#include "menu.h"

Menu::Menu()
{
    state = LOADING;
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
    NE_ModelSetMaterial(text, textMaterial);

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
    LoadButtons();

    // Load tiled BG
    NF_LoadTiledBg(TITLE_BG_NAME, TITLE_BG_NAME, 256, 256);
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

void Menu::LoadButtons()
{
    // Load button sprite
    NF_LoadSpriteGfx("sprite/buttons", 2, 64, 64);
    NF_LoadSpritePal("sprite/buttons", 2);
    NF_VramSpriteGfx(1, 2, 1, false);
    NF_VramSpritePal(1, 2, 1);

    // Load all buttons
    for (int i = 0; i < 4; i++)
    {
        // Create, position & scale sprite
        NF_CreateSprite(1, BUTTONS[i], 1, 1, BUTTON_COORDS[i][0], BUTTON_COORDS[i][1]);
        NF_EnableSpriteRotScale(1, BUTTONS[i], BUTTONS[i], true);
        NF_SpriteRotScale(1, BUTTONS[i], 0, 300, 300);
        NF_SpriteFrame(1, BUTTONS[i], i * 2);
    }
}

void Menu::SetState(MenuState newState, Sound *sound)
{

    if (newState == LOADING)
    {
        if (state != LOGO)
        {
            NF_DeleteTiledBg(1, TITLE_BG);
        }
        sound->StopBGM();
    }
    else if (state == LOGO && newState != LOGO)
    {
        NF_CreateTiledBg(1, TITLE_BG, TITLE_BG_NAME);
        sound->PlayBGM(BGM_TITLE_LOOP, true, 5);
    }
    else if (newState == LOGO)
    {
        if (!(state == LOADING || state == LOGO))
        {
            NF_DeleteTiledBg(1, TITLE_BG);
        }
        sound->PlayBGM(BGM_TITLE_HOOK, false, 0);
    }

    if (state == SOUND_TEST && newState == MENU)
    {
        sound->PlayBGM(BGM_TITLE_LOOP, true, 5);
        currentSoundTestTrack = 1;
    }

    currentSequenceIndex = 0;
    state = newState;
}

void Menu::PositionLogo()
{
    // Position logo
    NE_ModelSetCoordI(logo, 0, 0, 0);
    NE_ModelSetCoordI(text, 0, 0, 0);
    NE_ModelTranslate(logo, TARGET_X, Y, Z);
    NE_ModelTranslate(text, TARGET_TEXT_X, Y + 0.1, Z + 4.55);
}

void Menu::Update(volatile int frame, Sound *sound)
{
    NF_ClearTextLayer(1, 0);
    switch (state)
    {
    case LOADING:
        // Hide buttons and backgrounds
        NF_ShowSprite(1, START_SPRITE, false);
        for (int i = 0; i < 4; i++)
        {
            NF_ShowSprite(1, BUTTONS[i], false);
        }
        break;

    case LOGO:
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

        // Hide buttons
        for (int i = 0; i < 4; i++)
        {
            NF_ShowSprite(1, BUTTONS[i], false);
        }

        if (frame >= 690)
        {
            SetState(TITLE, sound);
            return;
        }
        break;

    case TITLE:
        NF_SpriteFrame(1, START_SPRITE, 2);

        // Hide buttons
        for (int i = 0; i < 4; i++)
        {
            NF_ShowSprite(1, BUTTONS[i], false);
        }

        if (frame % 30 == 0)
        {
            showStartSprite = !showStartSprite;
            NF_ShowSprite(1, START_SPRITE, showStartSprite);
        }
        break;

    case MENU:
        // Show buttons
        for (int i = 0; i < 4; i++)
        {
            NF_ShowSprite(1, BUTTONS[i], true);
            NF_SpriteFrame(1, BUTTONS[i], (i * 2) + (((i + 1) == highlightedItem) ? 1 : 0));
        }

        NF_ShowSprite(1, START_SPRITE, false);
        break;

    case RUMBLE_INFO:
        NF_ShowSprite(1, START_SPRITE, false);
        for (int i = 0; i < 4; i++)
        {
            NF_ShowSprite(1, BUTTONS[i], false);
        }

        if (isRumbleInserted())
        {
            NF_WriteText(1, 0, 1, 7, "Rumble Pak detected.");
            NF_WriteText(1, 0, 1, 8, "Rumble is now enabled.");
            NF_WriteText(1, 0, 1, 10, "To disable rumble, power off");
            NF_WriteText(1, 0, 1, 11, "the system and remove the");
            NF_WriteText(1, 0, 1, 12, "Rumble Pak from SLOT-2.");
            setRumble(frame % 2 == 0);
        }
        else
        {
            NF_WriteText(1, 0, 1, 8, "A Nintendo DS Rumble Pak is");
            NF_WriteText(1, 0, 1, 9, "required for this feature.");
            NF_WriteText(1, 0, 1, 11, "Please insert a Rumble Pak");
            NF_WriteText(1, 0, 1, 12, "into SLOT-2 to enable rumble.");
        }

        NF_WriteText(1, 0, 1, 15, "Touch the screen to continue.");
        break;

    case SOUND_TEST:
        NF_ShowSprite(1, START_SPRITE, false);
        for (int i = 0; i < 4; i++)
        {
            NF_ShowSprite(1, BUTTONS[i], false);
        }

        NF_WriteText(1, 0, 1, 5, "Breaking Bad DS - SOUND TEST");
        NF_WriteText(1, 0, 1, 6, "LEFT/RIGHT to change track.");
        NF_WriteText(1, 0, 1, 10, "Currently playing:");
        NF_WriteText(1, 0, 1, 11, BGM_NAMES[currentSoundTestTrack]);

        NF_WriteText(1, 0, 1, 15, "Touch the screen to continue.");
        break;
    }
}

MenuSelection Menu::HandleInput(Sound *sound)
{
    touchPosition touch;
    switch (state)
    {
    case LOGO:
        if (keysDown() & KEY_TOUCH || keysDown() & KEY_A || keysDown() & KEY_START)
        {
            return SKIP_LOGO;
        }
        return NONE;
    case TITLE:
        if (keysDown() & KEY_TOUCH || keysDown() & KEY_A || keysDown() & KEY_START)
        {
            SetState(MENU, sound);
        }
        return NONE;
    case SOUND_TEST:
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
            sound->PlayBGM(static_cast<BGM>(currentSoundTestTrack), true, 0);
        }

    case RUMBLE_INFO:
        if (keysDown() & KEY_TOUCH || keysDown() & KEY_B)
        {
            SetState(MENU, sound);
        }
        return NONE;
    case MENU:
        if (keysDown() & KEY_TOUCH)
        {
            touchRead(&touch);

            if (IsTouchInBox(BUTTON_COORDS[0], 64, touch))
            {
                return CheckSelection(START_GAME);
            }
            else if (IsTouchInBox(BUTTON_COORDS[1], 64, touch))
            {
                return CheckSelection(START_TUTORIAL);
            }
            else if (IsTouchInBox(BUTTON_COORDS[2], 32, touch))
            {
                if (CheckSelection(BACK_TO_TITLE))
                {
                    SetState(TITLE, sound);
                    return BACK_TO_TITLE;
                }
                return NONE;
            }
            else if (IsTouchInBox(BUTTON_COORDS[3], 32, touch))
            {
                if (CheckSelection(TOGGLE_RUMBLE))
                {
                    SetState(RUMBLE_INFO, sound);
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
                SetState(SOUND_TEST, sound);
                return NONE;
            }
            else if (currentSequenceIndex > 8)
            {
                return NONE;
            }
        }
        if (keysDown() & KEY_UP && !(highlightedItem == START_GAME || highlightedItem == START_TUTORIAL))
        {
            highlightedItem = (highlightedItem == TOGGLE_RUMBLE ? START_GAME : START_TUTORIAL);
        }
        else if (keysDown() & KEY_DOWN && !(highlightedItem == TOGGLE_RUMBLE || highlightedItem == BACK_TO_TITLE))
        {
            highlightedItem = (highlightedItem == START_GAME ? TOGGLE_RUMBLE : BACK_TO_TITLE);
        }
        else if (keysDown() & KEY_LEFT && !(highlightedItem == START_GAME || highlightedItem == TOGGLE_RUMBLE))
        {
            highlightedItem = (highlightedItem == START_TUTORIAL ? START_GAME : TOGGLE_RUMBLE);
        }
        else if (keysDown() & KEY_RIGHT && !(highlightedItem == BACK_TO_TITLE || highlightedItem == START_TUTORIAL))
        {
            highlightedItem = (highlightedItem == START_GAME ? START_TUTORIAL : BACK_TO_TITLE);
        }

        if (keysDown() & KEY_A || keysDown() & KEY_START)
        {
            switch (highlightedItem)
            {
            default:
                return CheckSelection(highlightedItem);
            case BACK_TO_TITLE:
                if (CheckSelection(BACK_TO_TITLE))
                {
                    SetState(TITLE, sound);
                    return BACK_TO_TITLE;
                }
                return NONE;
            case TOGGLE_RUMBLE:
                if (CheckSelection(TOGGLE_RUMBLE))
                {
                    SetState(RUMBLE_INFO, sound);
                    return TOGGLE_RUMBLE;
                }
                return NONE;
            }
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
    if (state != LOADING)
    {
        NE_ModelDraw(backdrop);
        NE_ModelDraw(logo);
        NE_ModelDraw(text);
    }
}

void Menu::Unload(Sound *sound)
{
    SetState(LOADING, sound);

    // Free the model and material
    NE_ModelDelete(logo);
    NE_ModelDelete(text);
    NE_ModelDelete(backdrop);

    NE_MaterialDelete(logoMaterial);
    NE_MaterialDelete(textMaterial);
    NE_MaterialDelete(backdropMaterial);

    // Delete sprites
    NF_DeleteSprite(1, START_SPRITE);
    for (int i = 0; i < 4; i++)
    {
        NF_DeleteSprite(1, BUTTONS[i]);
    }

    // Unload sprite assets
    NF_UnloadSpriteGfx(1);
    NF_UnloadSpritePal(1);
    NF_FreeSpriteGfx(1, 0);
    NF_UnloadSpriteGfx(2);
    NF_UnloadSpritePal(2);
    NF_FreeSpriteGfx(1, 1);

    // Unload tiled BG
    NF_UnloadTiledBg(TITLE_BG_NAME);
}