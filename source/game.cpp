#include "game.h"

Game::Game()
{
    // Initialize nitroFS before doing anything else
    consoleDemoInit();
    consoleClear();
    printf("Loading nitroFS. Use melonDS if this fails.\n");
    if (!nitroFSInit(NULL))
    {
        printf("Failed to start nitroFS\n");
        printf("Press POWER or RESET to continue\n");

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

    // Enable texture shading
    NE_ShadingEnable(true);

    // Setup lighting
    NE_LightSet(0, NE_White, 0, -1, -1);

    // Setup background color
    NE_ClearColorSet(CLEAR_COLOR, 31, 63);

    // Create camera
    camera = NE_CameraCreate();
}

void Game::UpdateCamera(volatile int frame)
{
    // Update the camera position based on mode
    switch (mode)
    {
    case TITLE_SCREEN:
        cameraTx = BASE_TITLE_CAMERA_POS[0];
        cameraTy = BASE_TITLE_CAMERA_POS[1];
        cameraTz = BASE_TITLE_CAMERA_POS[2];
        break;
    case DIALOGUE:
        cameraTx = player.x;
        cameraTy = 7.5;
        cameraTz = -2.5 + (player.z / 6);
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

void Game::StartGame(bool tutorialGame, int timeLimit, int batchQuota)
{
    frame = 0;
    isTutorial = tutorialGame;
    tutorialProgress = 0;
    mode = MOVE;

    // Prepare laboratory scene
    LoadLabScene();

    // Move player to starting position
    player.Translate(-11.5, 0, 4.5);
    player.targetX = 4;
    player.tileX = 4;
    player.targetZ = 1;
    player.tileZ = 1;

    // Setup camera
    cameraX = BASE_MOVE_CAMERA_POS[0];
    cameraY = BASE_MOVE_CAMERA_POS[1];
    cameraZ = BASE_MOVE_CAMERA_POS[2];
    NE_CameraSet(camera,
                 cameraX, cameraY, cameraZ,
                 -11.5, 1, 8.5,
                 0, 1, 0);

    // Start tutorial
    if (tutorialGame)
    {
        SetDialogue(GALE, SCRIPT_GALE_TUTORIAL_INTRO, SCRIPT_GALE_TUTORIAL_INTRO_LENGTH, frame);
        tutorialProgress++;
    }
}

void Game::LoadLabScene()
{
    // Load map and player
    if (map.Load() == -1)
    {
        WaitLoop();
    }
    if (player.Load() == -1)
    {
        WaitLoop();
    }

    // Set fog color
    NE_FogEnable(3, RGB15(15, 0, 0), 31, 3, 0x7c00);
}

// Unload map and player
void Game::UnLoadLabScene()
{
    map.Unload();
    player.Unload();
}

void Game::StartTitleScreen()
{
    frame = 0;
    mode = TITLE_SCREEN;

    // Load logo
    LoadLogoScene();

    // Position camera
    cameraX = BASE_TITLE_CAMERA_POS[0];
    cameraY = BASE_TITLE_CAMERA_POS[1];
    cameraZ = BASE_TITLE_CAMERA_POS[2];
    NE_CameraSet(camera,
                 cameraX, cameraY, cameraZ,
                 0, BASE_TITLE_CAMERA_POS[1], 0,
                 0, 1, 0);
}

void Game::LoadLogoScene()
{
    // Load logo
    if (logo.Load() == -1)
    {
        WaitLoop();
    }
    
    // Set fog color
    NE_FogEnable(3, NE_DarkGreen, 31, 3, 0x7c00);
}

void Game::UnLoadLogoScene()
{
    logo.Unload();
}

void Game::UpdateTitleScreen(volatile int frame)
{
    logo.Update(frame);

    // If the touch screen
    if (keysDown() & KEY_TOUCH)
    {
        // Start game
        UnLoadLogoScene();
        StartGame(false, 60, 10);
    }
}

void Game::SetDialogue(Speaker speaker, const char script[][128], int scriptLength, int startFrame)
{
    if (mode == DIALOGUE)
    {
        return;
    }
    mode = DIALOGUE;
    player.facing = DOWN;

    // Set dialog params
    currentSpeaker = speaker;
    currentLineIndex = 0;
    currentLineStartFrame = startFrame;
    currentSpeakerAnimation = 0;

    // copy script to currentScript
    for (int i = 0; i < scriptLength; i++)
    {
        strncpy(currentScript[i], script[i], 128);
    }
    currentScriptLength = scriptLength;

    // Set lab background
    NF_LoadTiledBg("bg/lab", "lab", 256, 256);
    NF_CreateTiledBg(1, 3, "lab");

    // Load sprite files from NitroFS
    switch (currentSpeaker)
    {
    case GALE:
        NF_LoadSpriteGfx("sprite/gale", 1, 64, 64);
        NF_LoadSpritePal("sprite/gale", 1);
        break;
    }
    NF_VramSpriteGfx(1, 1, 0, false); // This keeps unused frames in RAM
    NF_VramSpritePal(1, 1, 0);

    // Create, position & scale sprite
    NF_CreateSprite(1, currentSpeaker + 10, 0, 0, 64, 32);
    NF_EnableSpriteRotScale(1, currentSpeaker + 10, currentSpeaker + 10, true);
    NF_SpriteRotScale(1, currentSpeaker + 10, 0, 386, 386);
}

void Game::UpdateDialogue(volatile int frame, uint32 keys)
{
    uint charsToPrint = (frame - currentLineStartFrame) / 3;
    char currentLine[128];
    strncpy(currentLine, currentScript[currentLineIndex], 128);
    bool endOfLine = false;
    if (charsToPrint >= strlen(currentLine))
    {
        endOfLine = true;
        charsToPrint = strlen(currentLine);
    }

    // Update speaker animation
    if (!endOfLine && (frame - currentLineStartFrame) % 10 == 0)
    {
        currentSpeakerAnimation++;
        if (currentSpeakerAnimation > 7)
        {
            currentSpeakerAnimation = 0;
        }
        NF_SpriteFrame(1, currentSpeaker + 10, currentSpeakerAnimation);
    }

    // Handle input
    if (endOfLine && ((keys & KEY_A) || (keys & KEY_TOUCH)))
    {
        currentLineIndex++;
        currentLineStartFrame = frame;

        if (currentLineIndex >= currentScriptLength)
        {
            ClearDialogue();
            return;
        }
    }

    char lineToPrint[charsToPrint];
    strncpy(lineToPrint, currentLine, charsToPrint);
    NF_ClearTextLayer(1, 0);

    const int firstLineChars = 29;

    // Print dialogue lines
    int line1Chars = charsToPrint > firstLineChars ? firstLineChars : charsToPrint;
    char line1[line1Chars];
    strncpy(line1, lineToPrint, line1Chars);
    line1[line1Chars] = '\0';
    NF_WriteText(1, 0, 1, 21, line1);

    int line2Chars = charsToPrint > firstLineChars ? charsToPrint - firstLineChars : 0;
    if (line2Chars > 0)
    {
        char line2[line2Chars];
        strncpy(line2, lineToPrint + firstLineChars, line2Chars);
        line2[line2Chars] = '\0';
        NF_WriteText(1, 0, 1, 22, line2);
    }
}

void Game::ClearDialogue()
{
    if (mode != DIALOGUE)
    {
        return;
    }
    mode = MOVE;

    NF_DeleteTiledBg(1, 3);
    NF_DeleteSprite(1, currentSpeaker + 10);
    NF_ClearTextLayer(1, 0);

    // Unload from RAM, VRAM
    NF_UnloadSpriteGfx(1);
    NF_UnloadSpritePal(1);
    NF_FreeSpriteGfx(1, 0);
}

void Game::StartMinigame(Tile tile)
{
    switch (tile)
    {
    case MINIGAME_VALVE:
        currentMinigame = &valveMinigame;
        break;
    default:
        currentMinigame = NULL;
        return;
    }

    mode = MINIGAME;
    currentMinigame->Load();
}

void Game::DeleteMinigame()
{
    if (currentMinigame == NULL)
    {
        return;
    }
    currentMinigame->Delete();
    currentMinigame = NULL;
}

void Game::Render(volatile int frame)
{
    // Set poly format
    NE_PolyFormat(31, 0, NE_LIGHT_0, NE_CULL_NONE, NE_FOG_ENABLE);

    // Set camera
    NE_CameraUse(camera);

    // Render title screen
    if (mode == TITLE_SCREEN)
    {
        logo.Draw();
        return;
    }

    player.Update(frame);
    player.Move(map);
    map.RotateSecurityCamera(player.x, player.z);
    map.Draw();
    player.Draw();
}

void Game::Update(volatile int frame)
{
    // Wait for the VBlank interrupt
    NE_WaitForVBL(NE_UPDATE_ANIMATIONS);

    // Copy shadow OAM copy to the OAM of the 2D sub engine
    oamUpdate(&oamSub);

    // Debug - print debug info
    if (debugFlag && mode == MOVE)
    {
        player.PrintCoords(map);

        char modeText[100];
        sprintf(modeText, "Mode: %d", mode);
        NF_WriteText(1, 0, 1, 4, modeText);

        char memUsage[100];
        sprintf(memUsage, "Mem: %d (%d)", getMemUsed(), getMemFree());
        NF_WriteText(1, 0, 1, 5, memUsage);
    }

    // Update text layers
    NF_UpdateTextLayers();

    // Handle input or dialogue progress
    scanKeys();
    if (mode != TITLE_SCREEN && mode != DIALOGUE)
    {
        player.HandleInput(keysHeld());
    }
    else
    {
        UpdateDialogue(frame, keysDown());
    }

    // Update camera
    UpdateCamera(frame);

    // Tutorials check
    if (mode != TITLE_SCREEN)
    {
        // Update timer
        if (frame % 60 == 0 && timeLimit > -1)
        {
            timeLimit--;

            // todo draw time limit to HUD

            if (timeLimit == 0)
            {
                // TODO - game over
                return;
            }
        }

        switch (map.GetTileAt(player.tileX, player.tileZ))
        {
        case MINIGAME_VALVE:
            if (isTutorial && tutorialProgress == 1)
            {
                SetDialogue(GALE, SCRIPT_GALE_TUTORIAL_VALVE, SCRIPT_GALE_TUTORIAL_VALVE_LENGTH, frame);
                tutorialProgress++;
                break;
            }
            if (mode == MOVE)
            {
                StartMinigame(MINIGAME_VALVE);
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
    }
    else
    {
        UpdateTitleScreen(frame);
    }

    // Check for minigame
    if (mode == MINIGAME)
    {
        currentMinigame->Update(frame, keysHeld());
    }

    // Refresh shadow OAM copy
    NF_SpriteOamSet(1);
}