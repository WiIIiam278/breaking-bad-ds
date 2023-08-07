#include "game.h"

Game::Game()
{
    // Initialize nitroFS before doing anything else
    consoleDemoInit();
    consoleClear();
    printf("\n\n\n\n\n\n\n\n\n\n\n    Getting Ready to Cook...\n");
    if (!nitroFSInit(NULL))
    {
        printf("\x1B[31mFailed to start nitroFS\n");
        printf("\x1B[31mPlease reset the system\n");

        while (1)
        {
            swiWaitForVBlank();
        }
    }
    swiWaitForVBlank();

    // Set the root folder to the nitroFS filesystem
    NF_SetRootFolder("NITROFS");

    // Setup interrupt handlers
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    // Setup sound
    sound.LoadSound();
}

void Game::WaitLoop()
{
    while (1)
    {
        swiWaitForVBlank();
    }
}

void Game::Prepare2DGraphics()
{
    // Initialize sub 2D engine
    NF_Set2D(1, 0);

    // Initialize sprites for sub screen (it uses VRAM D and I)
    NF_InitSpriteBuffers();
    NF_InitSpriteSys(1);

    // Initialize tiled backgrounds and text systems for the 2D sub engine (it
    // uses VRAM C and H)
    NF_InitTiledBgBuffers();
    NF_InitTiledBgSys(1);
    NF_InitTextSys(1);

    // Load assets from filesystem to RAM
    NF_LoadTextFont("font/default", "normal", 256, 256, 0);

    // Create text layer
    NF_CreateTextLayer(1, 0, 0, "normal");
}

void Game::Prepare3DGraphics()
{
    // When using nflib for the 2D sub screen engine, banks C and H are used for
    // backgrounds and banks D and I are used for sprites. Nitro Engine only
    // uses bank E for palettes, so the only thing we need to do is to tell
    // Nitro Engine to only use banks A and B and leave C and D unused.
    NE_Init3D();

    // Reset the texture system
    NE_TextureSystemReset(0, 0, NE_VRAM_AB);

    // Setup lighting
    NE_LightSet(0, NE_White, -0.5, -0.5, -0.5);

    // Setup background color
    NE_ClearColorSet(CLEAR_COLOR, 31, 63);

    // Enable shading and outlining
    NE_ShadingEnable(true);
    NE_OutliningEnable(true);
    NE_OutliningSetColor(1, NE_Black);     // Singleplayer player outline
    NE_OutliningSetColor(2, NE_Green);     // Multiplayer Player 1 outline
    NE_OutliningSetColor(3, NE_LightBlue); // Multiplayer: Player 2 outline

    // Create camera
    camera = NE_CameraCreate();
}

void Game::UpdateCamera()
{
    // Update the camera position based on mode
    switch (mode)
    {
    case MAIN_MENU:
        cameraTx = BASE_TITLE_CAMERA_POS[0];
        cameraTy = BASE_TITLE_CAMERA_POS[1];
        cameraTz = BASE_TITLE_CAMERA_POS[2];
        break;
    case DIALOGUE:
    case PAUSED:
        cameraTx = player.x;
        cameraTy = 7.5;
        cameraTz = -2.5 + (player.z / 6);
        break;
    case GAME_OVER:
        if (gameOverFrame < -10)
            return;
        cameraTx = player.x - 0.5f;
        cameraTy = 5 + (((float)gameOverFrame / 500.0f) * 13.0f);
        cameraTz = player.z + 1.5f;
        NE_CameraRotate(camera, 0, frame % 2 == 0 ? 1 : 0, 0);
        break;
    default:
        cameraTx = player.x;
        cameraTy = BASE_MOVE_CAMERA_POS[1] + (player.z / 6);
        cameraTz = BASE_MOVE_CAMERA_POS[2] + (player.z / 6);
        break;
    }

    // Move camera towards target
    float camXDiff = cameraTx - cameraX;
    float camYDiff = cameraTy - cameraY;
    float camZDiff = cameraTz - cameraZ;
    float camXSpeed = camXDiff / 10;
    float camYSpeed = camYDiff / 10;
    float camZSpeed = camZDiff / 10;

    NE_CameraMove(camera, camXSpeed, camYSpeed, camZSpeed);
    cameraX += camXSpeed;
    cameraY += camYSpeed;
    cameraZ += camZSpeed;
}

void Game::TranslateCamera(float x, float y, float z)
{
    cameraTx += x;
    cameraTy += y;
    cameraTz += z;
}

void Game::Transition(bool fadeIn, int duration, TransitionScreen screen)
{
    for (int i = 0; i < 2; i++)
    {
        if (!(screen == i || screen == TS_BOTH))
        {
            continue;
        }
        if (duration <= 0)
        {
            setBrightness(i + 1, fadeIn ? 0 : -16);
            isTransitioning[i] = false;
            continue;
        }

        isTransitioning[i] = true;
        isFadingIn[i] = fadeIn;
        transitionDuration[i] = duration;
        transitionStartFrame[i] = frame;
    }
}

void Game::UpdateTransition()
{
    for (int i = 0; i < 2; i++)
    {
        if (!isTransitioning[i])
        {
            continue;
        }

        if (frame >= (transitionStartFrame[i] + transitionDuration[i]))
        {
            setBrightness(i + 1, isFadingIn ? 0 : -16);
            isTransitioning[i] = false;
            return;
        }

        int brightness;
        if (!isFadingIn)
        {
            brightness = (frame - transitionStartFrame[i]) * 16 / transitionDuration[i];
        }
        else
        {
            brightness = 16 - (frame - transitionStartFrame[i]) * 16 / transitionDuration[i];
        }
        setBrightness(i + 1, -brightness);
    }
}

void Game::TogglePauseMenu()
{
    if (!(mode == MOVE || mode == PAUSED))
    {
        return;
    }
    bool pausing = mode == MOVE;
    Transition(!pausing, 0, TS_BOTTOM);

    sound.PlaySFX(SFX_MENU_SELECT);
    if (pausing)
    {
        player.canMove = false;
        player.walking = false;
        ToggleHud(false);
        NF_LoadTiledBg(PAUSED_BG_NAME, PAUSED_BG_NAME, 256, 256);
        NF_CreateTiledBg(1, PAUSED_BG, PAUSED_BG_NAME);
        mode = PAUSED;
    }
    else
    {
        NF_DeleteTiledBg(1, PAUSED_BG);
        NF_UnloadTiledBg(PAUSED_BG_NAME);
        ToggleHud(true);
        player.canMove = true;
        mode = MOVE;
    }

    Transition(pausing, 15, TS_BOTTOM);
}

// Show/hide bottom screen HUD
void Game::ToggleHud(bool show)
{
    bool isMultiplayer = gameType == MULTIPLAYER_GAME;
    if (show && !hudVisible)
    {
        NF_LoadTiledBg(isMultiplayer ? MP_HUD_BG_NAME : SP_HUD_BG_NAME, isMultiplayer ? MP_HUD_BG_NAME : SP_HUD_BG_NAME, 256, 256);
        NF_CreateTiledBg(1, HUD_BG, isMultiplayer ? MP_HUD_BG_NAME : SP_HUD_BG_NAME);

        // Player markers
        int playerSpriteFrame = isMultiplayer ? isHostClient() ? 1 : 2 : 0;
        NF_CreateSprite(1, HUD_MAP_PLAYER_SPRITE, HUD_MAP_ICONS + 1, HUD_MAP_ICONS + 1, HUD_MAP_ORIGIN_COORDS[0], HUD_MAP_ORIGIN_COORDS[1]);
        NF_SpriteFrame(1, HUD_MAP_PLAYER_SPRITE, playerSpriteFrame);
        if (playerSpriteFrame > 0)
        {
            NF_CreateSprite(1, HUD_MAP_PLAYER2_SPRITE, HUD_MAP_ICONS + 1, HUD_MAP_ICONS + 1, HUD_MAP_ORIGIN_COORDS[0], HUD_MAP_ORIGIN_COORDS[1]);
            NF_SpriteFrame(1, HUD_MAP_PLAYER2_SPRITE, playerSpriteFrame == 1 ? 2 : 1);
        }

        // Map marker
        NF_CreateSprite(1, HUD_MAP_MARKER_SPRITE, HUD_MAP_ICONS + 1, HUD_MAP_ICONS + 1, HUD_MAP_ORIGIN_COORDS[0], HUD_MAP_ORIGIN_COORDS[1]);
        NF_SpriteFrame(1, HUD_MAP_MARKER_SPRITE, 3);

        // Checkboxes
        for (int i = 0; i < HUD_CHECKBOX_COUNT; i++)
        {
            NF_CreateSprite(1, HUD_CHECKBOX_SPRITES[i], HUD_CHECKBOXES + 1, HUD_CHECKBOXES + 1, HUD_CHECKBOXES_COORDS[0] + (i / 2), HUD_CHECKBOXES_COORDS[1] + (i * HUD_CHECKBOX_Y_SPACING));
            NF_SpriteFrame(1, HUD_CHECKBOX_SPRITES[i], 0);
        }

        // Timer and purity
        for (int i = 0; i < 3; i++)
        {
            NF_CreateSprite(1, HUD_TIMER_SPRITES[i], HUD_NUMBERS + 1, HUD_NUMBERS + 1, HUD_TIMER_COORDS[0] + (i * 16), HUD_TIMER_COORDS[1]);
            NF_SpriteFrame(1, HUD_TIMER_SPRITES[i], 0);
            NF_CreateSprite(1, HUD_PURITY_SPRITES[i], HUD_NUMBERS + 1, HUD_NUMBERS + 1, HUD_PURITY_COORDS[0] + (i * 16), HUD_PURITY_COORDS[1]);
            NF_SpriteFrame(1, HUD_PURITY_SPRITES[i], 0);
        }

        // Quota / Multiplayer Vs. Score
        for (int i = 0; i < 4; i++)
        {
            NF_CreateSprite(1, HUD_QUOTA_SPRITES[i], HUD_NUMBERS + 1, HUD_NUMBERS + 1, (HUD_QUOTA_COORDS[0] + (i >= 2 ? 12 : 0)) + (i * 16), HUD_QUOTA_COORDS[1]);
            NF_SpriteFrame(1, HUD_QUOTA_SPRITES[i], 0);
        }
    }
    else if (hudVisible)
    {
        NF_DeleteTiledBg(1, HUD_BG);
        NF_UnloadTiledBg(isMultiplayer ? MP_HUD_BG_NAME : SP_HUD_BG_NAME);
        NF_DeleteSprite(1, HUD_MAP_PLAYER_SPRITE);
        if (isMultiplayer)
        {
            NF_DeleteSprite(1, HUD_MAP_PLAYER2_SPRITE);
        }
        NF_DeleteSprite(1, HUD_MAP_MARKER_SPRITE);
        for (int i = 0; i < HUD_CHECKBOX_COUNT; i++)
        {
            NF_DeleteSprite(1, HUD_CHECKBOX_SPRITES[i]);
        }
        for (int i = 0; i < 3; i++)
        {
            NF_DeleteSprite(1, HUD_TIMER_SPRITES[i]);
            NF_DeleteSprite(1, HUD_PURITY_SPRITES[i]);
        }
        for (int i = 0; i < 4; i++)
        {
            NF_DeleteSprite(1, HUD_QUOTA_SPRITES[i]);
        }
    }
    hudVisible = show;
}

void Game::UpdateHud()
{
    bool isMultiplayer = gameType == MULTIPLAYER_GAME;
    if (mode == MOVE)
    {
        NF_MoveSprite(1, HUD_MAP_PLAYER_SPRITE, HUD_MAP_ORIGIN_COORDS[0] - (player.x * 4.1), HUD_MAP_ORIGIN_COORDS[1] - (player.z * 4.4));
        if (isMultiplayer)
        {
            NF_MoveSprite(1, HUD_MAP_PLAYER2_SPRITE, HUD_MAP_ORIGIN_COORDS[0] - (player2->x * 4.1), HUD_MAP_ORIGIN_COORDS[1] - (player2->z * 4.4));
        }
        NF_MoveSprite(1, HUD_MAP_MARKER_SPRITE, HUD_MAP_ORIGIN_COORDS[0] - (HUD_BATCH_PROGRESS_MARKER_COORDS[currentBatchProgress][0] * 4.1), HUD_MAP_ORIGIN_COORDS[1] - (HUD_BATCH_PROGRESS_MARKER_COORDS[currentBatchProgress][1] * 4.4));
        for (int i = 0; i < HUD_CHECKBOX_COUNT; i++)
        {
            if (isMultiplayer)
            {
                bool isHost = isHostClient();
                bool p1Completed = isHost ? (currentBatchProgress > i) : (getOpponent()->currentBatchStep > i);
                bool p2Completed = isHost ? (getOpponent()->currentBatchStep > i) : (currentBatchProgress > i);
                int totalProgress = (p1Completed && p2Completed) ? 4 : p1Completed && !p2Completed ? 2
                                                                   : !p1Completed && p2Completed   ? 3
                                                                                                   : 0;
                NF_SpriteFrame(1, HUD_CHECKBOX_SPRITES[i], totalProgress);
            }
            else
            {
                NF_SpriteFrame(1, HUD_CHECKBOX_SPRITES[i], currentBatchProgress > i ? 1 : 0);
            }
        }
        for (int i = 0; i < 3; i++)
        {
            if (timeLimit > -1)
            {
                NF_SpriteFrame(1, HUD_TIMER_SPRITES[i], (int)(timeLimit / pow(10, 2 - i)) % 10);
            }
            NF_SpriteFrame(1, HUD_PURITY_SPRITES[i], (int)(batchPurity / pow(10, 2 - i)) % 10);
        }
        for (int i = 0; i < 4; i++)
        {
            NF_SpriteFrame(1, HUD_QUOTA_SPRITES[i], (int)((i < 2 ? batchesComplete : (isMultiplayer ? player2batchesComplete : batchQuota)) / pow(10, 3 - i)) % 10);
        }
    }
}

void Game::QuitToTitle()
{
    if (isQuitting)
    {
        return;
    }
    isQuitting = true;
    if (gameType == MULTIPLAYER_GAME)
    {
        disableMultiplayer();
    }
    UnLoadLabScene();
    NE_SpecialEffectSet(NE_NONE);
    StartMenuScreen(false);
    sound.StopBGM();
    sound.PlaySFX(SFX_MENU_SELECT);
}

void Game::StartDialogue(ScriptId script)
{
    if (mode == DIALOGUE)
    {
        return;
    }
    Transition(false, 0, TS_BOTTOM);
    if (mode == MINIGAME)
    {
        currentMinigame->Unload();
    }
    mode = DIALOGUE;
    ToggleHud(false);
    player.facing = DOWN;
    dialogue.Load(script, frame);
    Transition(true, 30, TS_BOTTOM);
}

void Game::EndDialogue()
{
    if (mode != DIALOGUE)
    {
        return;
    }
    Transition(false, 0, TS_BOTTOM);
    dialogue.Unload();
    ToggleHud(true);
    mode = MOVE;
    Transition(true, 30, TS_BOTTOM);
}

void Game::CheckTutorials()
{
    if (gameType == TUTORIAL_GAME)
    {
        int dialogueId = dialogue.GetTutorialDialogue(tutorialProgress, currentBatchProgress, player.GetTile(map));
        if (dialogueId != -1)
        {
            StartDialogue((ScriptId)dialogueId);
            tutorialProgress++;
        }
    }
}

void Game::StartGame(GameType gameType, int timeLimit, int batchQuota)
{
    Transition(false, 0, TS_BOTH);

    frame = 0;
    isQuitting = false;
    tutorialProgress = 0;
    mode = MOVE;
    currentBatchProgress = 0;
    batchPurity = 100;
    batchesComplete = 0;

    this->gameType = gameType;
    this->timeLimit = timeLimit;
    this->batchQuota = batchQuota;

    // Prepare laboratory scene
    LoadLabScene();

    // Player setup
    if (gameType == MULTIPLAYER_GAME)
    {
        bool isHost = isHostClient();
        player.Translate(isHost ? -11.5 : -9.0, 0, 4.5);
        player.targetX = player.tileX = isHost ? 4 : 3;

        player2->Translate(!isHost ? -11.5 : -9.0, 0, 4.5);
        player2->targetX = player2->tileX = !isHost ? 4 : 3;
        player2->targetZ = player2->tileZ = 1;
    }
    else
    {
        player.Translate(-11.5, 0, 4.5);
        player.targetX = player.tileX = 4;
    }
    player.targetZ = player.tileZ = 1;

    // Setup camera
    cameraX = BASE_MOVE_CAMERA_POS[0];
    cameraY = BASE_MOVE_CAMERA_POS[1];
    cameraZ = BASE_MOVE_CAMERA_POS[2];
    NE_CameraSet(camera,
                 cameraX, cameraY, cameraZ,
                 -11.5, 1, 8.5,
                 0, 1, 0);

    sound.PlayBGM(BGM_THE_COUSINS, true);

    // Start tutorial
    CheckTutorials();
    Transition(true, 60, TS_BOTH);
}

void Game::LoadLabScene()
{
    bool isMultiplayer = gameType == MULTIPLAYER_GAME;

    // Load map and player
    if (map.Load() == -1)
    {
        WaitLoop();
    }
    
    Character p1Char = (isMultiplayer ? (!isHostClient() ? CHAR_JESSIE : CHAR_WALT) : ((keysHeld() & KEY_Y) && (keysHeld() & KEY_SELECT) ? CHAR_YEPPERS : CHAR_WALT));
    if (player.Load(p1Char) == -1)
    {
        WaitLoop();
    }
    if (isMultiplayer)
    {
        player2 = new Player();
        player2->isPlayer2 = true;
        if (player2->Load(isHostClient() ? CHAR_JESSIE : CHAR_WALT) == -1)
        {
            WaitLoop();
        }
    }

    // Set fog color
    NE_FogEnable(3, RGB15(15, 0, 0), 31, 3, 0x7c00);

    // Load sprites
    NF_LoadSpriteGfx("sprite/quality", QUALITY_INDICATOR_SPRITE, 64, 64);
    NF_LoadSpritePal("sprite/quality", QUALITY_INDICATOR_SPRITE);
    NF_VramSpriteGfx(1, QUALITY_INDICATOR_SPRITE, QUALITY_INDICATOR_SPRITE + 1, false);
    NF_VramSpritePal(1, QUALITY_INDICATOR_SPRITE, QUALITY_INDICATOR_SPRITE + 1);

    NF_LoadSpriteGfx("sprite/map_icons", HUD_MAP_ICONS, 16, 16);
    NF_LoadSpritePal("sprite/map_icons", HUD_MAP_ICONS);
    NF_VramSpriteGfx(1, HUD_MAP_ICONS, HUD_MAP_ICONS + 1, false);
    NF_VramSpritePal(1, HUD_MAP_ICONS, HUD_MAP_ICONS + 1);

    NF_LoadSpriteGfx("sprite/menu_checkboxes", HUD_CHECKBOXES, 16, 16);
    NF_LoadSpritePal("sprite/menu_checkboxes", HUD_CHECKBOXES);
    NF_VramSpriteGfx(1, HUD_CHECKBOXES, HUD_CHECKBOXES + 1, false);
    NF_VramSpritePal(1, HUD_CHECKBOXES, HUD_CHECKBOXES + 1);

    NF_LoadSpriteGfx("sprite/segment_numbers", HUD_NUMBERS, 16, 16);
    NF_LoadSpritePal("sprite/segment_numbers", HUD_NUMBERS);
    NF_VramSpriteGfx(1, HUD_NUMBERS, HUD_NUMBERS + 1, false);
    NF_VramSpritePal(1, HUD_NUMBERS, HUD_NUMBERS + 1);

    // Load HUD
    ToggleHud(true);
}

// Unload map and player
void Game::UnLoadLabScene()
{
    Transition(false, 0, TS_BOTH);
    ToggleHud(false);
    map.Unload();
    player.Unload();

    // Unload multiplayer
    if (mode == MULTIPLAYER_GAME)
    {
        player2->Unload();
    }

    // Remove fog
    NE_FogDisable();

    // Free sprites
    NF_FreeSpriteGfx(1, QUALITY_INDICATOR_SPRITE + 1);
    NF_UnloadSpriteGfx(QUALITY_INDICATOR_SPRITE);
    NF_UnloadSpritePal(QUALITY_INDICATOR_SPRITE);
    NF_FreeSpriteGfx(1, HUD_MAP_ICONS + 1);
    NF_UnloadSpriteGfx(HUD_MAP_ICONS);
    NF_UnloadSpritePal(HUD_MAP_ICONS);
    NF_FreeSpriteGfx(1, HUD_CHECKBOXES + 1);
    NF_UnloadSpriteGfx(HUD_CHECKBOXES);
    NF_UnloadSpritePal(HUD_CHECKBOXES);
    NF_FreeSpriteGfx(1, HUD_NUMBERS + 1);
    NF_UnloadSpriteGfx(HUD_NUMBERS);
    NF_UnloadSpritePal(HUD_NUMBERS);
}

void Game::StartMenuScreen(bool debugMode)
{
    Transition(false, 0, TS_BOTH);

    debugFlag = debugMode;
    frame = 0;
    mode = MAIN_MENU;

    // Quick-start debug game if the flag is set
    if (debugFlag)
    {
        StartGame(SINGLEPLAYER_GAME, 999, 1);
        currentBatchProgress = 3;
        return;
    }

    // Load logo
    LoadLogoScene();

    Transition(true, 120, TS_BOTH);
}

void Game::LoadLogoScene()
{
    // Position camera
    cameraX = BASE_TITLE_CAMERA_POS[0];
    cameraY = BASE_TITLE_CAMERA_POS[1];
    cameraZ = BASE_TITLE_CAMERA_POS[2];
    NE_CameraSet(camera,
                 cameraX, cameraY + 1.5f, cameraZ,
                 0, BASE_TITLE_CAMERA_POS[1], 0,
                 0, 1, 0);

    // Load logo
    if (menu.Load() == -1)
    {
        WaitLoop();
    }
    menu.SetState(MENU_LOGO, &sound);
}

void Game::UnLoadLogoScene()
{
    Transition(false, 0, TS_BOTH);
    menu.Unload(&sound);
}

void Game::UpdateMenuScreen()
{
    menu.Update(frame, &sound);

    switch (menu.HandleInput(&sound))
    {
    case SKIP_LOGO:
        frame = 689;
        menu.PositionLogo();
        break;
    case START_TUTORIAL:
        UnLoadLogoScene();
        StartGame(TUTORIAL_GAME, -1, 1);
        break;
    case START_1P_GAME:
        UnLoadLogoScene();
        StartGame(SINGLEPLAYER_GAME, 340, 10);
        break;
    case START_MP_GAME:
        UnLoadLogoScene();
        StartGame(MULTIPLAYER_GAME, 300, 99);
        break;
    }
}

void Game::StartMinigame(Tile tile)
{
    CheckTutorials();
    if (mode == MINIGAME || mode == DIALOGUE)
    {
        return;
    }

    if (tile > 2 && currentBatchProgress != ((int)tile - 3))
    {
        currentMinigame = NULL;
        delete currentMinigame;
        return;
    }

    switch (tile)
    {
    case MINIGAME_VALVE:
        currentMinigame = new ValveMinigame();
        break;
    case MINIGAME_PESTLE:
        currentMinigame = new PestleMinigame();
        break;
    case MINIGAME_PIPETTE:
        currentMinigame = new PipetteMinigame();
        break;
    case MINIGAME_MIX:
        currentMinigame = new MixMinigame();
        break;
    case MINIGAME_POUR:
        currentMinigame = new PourMinigame();
        break;
    case MINIGAME_CRACK:
        currentMinigame = new CrackMinigame();
        break;
    default:
        currentMinigame = NULL;
        delete currentMinigame;
        return;
    }

    Transition(false, 0, TS_BOTTOM);
    mode = MINIGAME;
    ToggleHud(false);
    currentMinigame->Load();
    Transition(true, 30, TS_BOTTOM);
}

void Game::DeleteMinigame()
{
    if (currentMinigame == NULL)
    {
        return;
    }
    Transition(false, 0, TS_BOTTOM);
    currentMinigame->Unload();
    currentMinigame = NULL;
    ToggleHud(true);
    Transition(true, 30, TS_BOTTOM);
}

void Game::ShowMinigameResult(MinigameResult indicator, int frames)
{
    int animation = indicator;
    showingIndicatorFor = frames;

    NF_CreateSprite(1, QUALITY_INDICATOR_SPRITE, QUALITY_INDICATOR_SPRITE + 1, QUALITY_INDICATOR_SPRITE + 1, 64, 32);
    NF_SpriteFrame(1, QUALITY_INDICATOR_SPRITE, animation);
    NF_EnableSpriteRotScale(1, QUALITY_INDICATOR_SPRITE, QUALITY_INDICATOR_SPRITE, true);
    NF_SpriteRotScale(1, QUALITY_INDICATOR_SPRITE, 0, 300, 300);

    if (indicator == GOOD)
    {
        sound.PlaySFX(SFX_SUCCESS_BELL);
    }
}

void Game::StartGameOver()
{
    if (!(mode == MOVE || mode == MINIGAME))
    {
        return;
    }
    player.walking = false;
    Transition(false, 0, TS_BOTH);

    // Clear other elements
    if (mode == MINIGAME)
    {
        DeleteMinigame();
    }
    else
    {
        ToggleHud(false);
    }

    // Set mode
    sound.StopBGM();
    NE_SpecialEffectSet(NE_NONE);
    mode = GAME_OVER;
    gameOverFrame = -170;
    player.SetLyingDown(true);
    NE_CameraRotate(camera, 90, 0, 0);
}

void Game::UpdateGameOver()
{
    gameOverFrame++;

    switch (gameOverFrame)
    {
    case -150:
        sound.PlaySFX(SFX_GOODBYE_WALTER);
        break;
    case -55:
    case -56:
    case -57:
    case -58:
    case -59:
    case -60:
        setRumble(gameOverFrame % 2 == 0);
        break;
    case -10:
        sound.StopSFX();
        break;
    case 0:
        sound.PlayBGM(BGM_BABY_BLUE, false);
        Transition(true, 60, TS_BOTH);
        break;
    case 520:
        Transition(false, 0, TS_BOTH);
        break;
    case 620:
        QuitToTitle();
        break;
    }
}

void Game::Tick()
{
    frame++;
}

void Game::Render()
{
    // Set camera
    NE_CameraUse(camera);

    // Set poly format
    NE_PolyFormat(31, 0, NE_LIGHT_0, NE_CULL_NONE, NE_FOG_ENABLE);

    // Render title screen
    if (mode == MAIN_MENU)
    {
        menu.Draw(frame);
        return;
    }

    // Update objects
    if (mode != PAUSED)
    {
        player.Update(frame);
        player.Move(map);

        if (gameType == MULTIPLAYER_GAME)
        {
            player2->Update(frame);
            player2->Move(map);
        }

        map.RotateSecurityCamera(player.x, player.z);
    }

    // Draw objects
    map.Draw();
    player.Draw(gameType == MULTIPLAYER_GAME ? (isHostClient() ? 1 : 2) : 0);
    if (gameType == MULTIPLAYER_GAME && (mode != GAME_OVER))
    {
        player2->Draw(!isHostClient() ? 1 : 2);
    }
}

void Game::Update()
{
    if (mode != MAIN_MENU && gameType == MULTIPLAYER_GAME && !isQuitting)
    {
        if (getMultiplayerStatus() == MP_CONNECTION_LOST)
        {
            QuitToTitle(); // todo display that connection was lost
        }
        else
        {
            Client *local = getLocalClient();
            local->playerTargetX = player.targetX;
            local->playerTargetZ = player.targetZ;
            local->playerTileX = player.tileX;
            local->playerTileZ = player.tileZ;
            local->playerFacing = player.rotation;
            local->batchesComplete = batchesComplete;
            local->currentBatchStep = currentBatchProgress;
            if (isHostClient())
            {
                local->timeLeft = timeLimit;
            }

            Client *opponent = getOpponent();
            if (opponent != NULL)
            {
                player2->targetX = opponent->playerTargetX;
                player2->targetZ = opponent->playerTargetZ;
                player2->tileX = opponent->playerTileX;
                player2->tileZ = opponent->playerTileZ;
                player2->rotation = opponent->playerFacing;
                player2batchesComplete = opponent->batchesComplete;
                player2BatchProgress = opponent->currentBatchStep;
                if (!isHostClient())
                {
                    timeLimit = opponent->timeLeft;
                }
            }

            tickMultiplayer();
        }
    }

    // Wait for the VBlank interrupt
    NE_WaitForVBL(NE_UPDATE_ANIMATIONS);

    // Copy shadow OAM copy to the OAM of the 2D sub engine
    oamUpdate(&oamSub);

    // Debug - print debug info
    if (debugFlag)
    {
        if (mode != MAIN_MENU && mode != MINIGAME && mode != DIALOGUE)
        {
            player.PrintCoords(map);
        }

        char modeText[100];
        sprintf(modeText, "Mode: %d", mode);
        NF_WriteText(1, 0, 1, 4, modeText);

        char memUsage[100];
        sprintf(memUsage, "Mem: %d (%d)", getMemUsed(), getMemFree());
        NF_WriteText(1, 0, 1, 5, memUsage);
    }

    // Update transition
    UpdateTransition();

    // Update text layers
    NF_UpdateTextLayers();

    // Handle input
    scanKeys();
    if (mode != MAIN_MENU && mode != DIALOGUE && mode != GAME_OVER)
    {
        if (mode == MOVE || mode == MINIGAME)
        {
            player.HandleInput(keysHeld());
        }

        if (gameType != MULTIPLAYER_GAME &&
            (mode == PAUSED) && (keysDown() & KEY_B))
        {
            NF_DeleteTiledBg(1, PAUSED_BG);
            NF_UnloadTiledBg(PAUSED_BG_NAME);
            QuitToTitle();
            return;
        }
        else if (gameType != MULTIPLAYER_GAME &&
                 (mode == MOVE || mode == PAUSED) && (keysDown() & KEY_START))
        {
            TogglePauseMenu();
        }
        else if (mode == MOVE && !player.walking && keysHeld() == 0)
        {
            idleFrames++;

            if (idleFrames == 4000)
            {
                player.facing = DOWN;
            }
            if (gameType == TUTORIAL_GAME && idleFrames > 6000)
            {
                idleFrames = 0;
                StartDialogue(SCRIPT_GALE_TUTORIAL_IDLE);
            }
        }
        else
        {
            idleFrames = 0;
        }

        // Update status indicators
        if (showingIndicatorFor > 0)
        {
            showingIndicatorFor--;
            if (showingIndicatorFor == 0)
            {
                NF_DeleteSprite(1, QUALITY_INDICATOR_SPRITE);
                CheckTutorials();
            }
        }
    }

    // Update dialogue
    if (mode == DIALOGUE)
    {
        if (dialogue.Update(frame, keysDown(), &sound))
        {
            sound.PlaySFX(SFX_MENU_DRUM);
            EndDialogue();
        }
    }

    // Update camera
    UpdateCamera();

    // Update timer
    if (mode != MAIN_MENU && mode != GAME_OVER && mode != PAUSED)
    {
        // Update timer
        if (frame % 60 == 0 && timeLimit > -1 && mode != DIALOGUE)
        {
            if (gameType != MULTIPLAYER_GAME || isHostClient())
            {
                timeLimit--;
            }

            if (batchesComplete < batchQuota)
            {
                if (timeLimit < 30)
                {
                    NE_SpecialEffectNoiseConfig((31 - timeLimit) / 4);
                    NE_SpecialEffectSet(NE_NOISE);
                }

                if (timeLimit < 10)
                {
                    sound.PlaySFX(SFX_MENU_DRUM);
                }

                if (timeLimit == 0)
                {
                    StartGameOver();
                    return;
                }
            }
        }

        Tile tile = map.GetTileAt(player.tileX, player.tileZ);
        switch (tile)
        {
        case MINIGAME_VALVE:
        case MINIGAME_PESTLE:
        case MINIGAME_PIPETTE:
        case MINIGAME_MIX:
        case MINIGAME_POUR:
        case MINIGAME_CRACK:
            if (mode == MOVE)
            {
                StartMinigame(tile);
            }
            break;
        default:
            if (mode == MINIGAME)
            {
                mode = MOVE;
                DeleteMinigame();
            }
            break;
        }

        if (hudVisible)
        {
            UpdateHud();
        }
    }
    else
    {
        UpdateMenuScreen();
    }

    // Check for minigame
    if (mode == MINIGAME)
    {
        inMinigameFor++;
        currentMinigame->Update(frame, keysHeld(), &sound);
        if (currentMinigame->IsComplete() && currentBatchProgress == ((int)player.GetTile(map) - 3))
        {
            MinigameResult result = currentMinigame->GetResult(inMinigameFor);
            ShowMinigameResult(result, 90);
            currentBatchProgress++;
            if (result == BAD)
            {
                batchPurity -= (16 + (frame % 5));
            }
            else if (result == OKAY)
            {
                batchPurity -= (6 + (frame % 3));
            }

            if (batchPurity < 5)
            {
                batchPurity = 5;
            }
        }
    }
    else
    {
        inMinigameFor = 0;
    }

    if (mode == GAME_OVER)
    {
        UpdateGameOver();
    }

    // Update sounds
    sound.Update(frame);

    // Refresh shadow OAM copy
    NF_SpriteOamSet(1);
}