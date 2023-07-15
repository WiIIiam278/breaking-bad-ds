#include "game.h"

Game::Game()
{
    // Initialize nitroFS before doing anything else
    NF_Set2D(0, 0);
    NF_Set2D(1, 0);
    consoleDemoInit();
    printf("Starting nitroFS...\n");
    if (!nitroFSInit(NULL))
    {
        printf("Failed to start nitroFS\n");
        printf("Press POWER or RESET to continue.\n");

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

void Game::Prepare3DGraphics()
{
    // When using nflib for the 2D sub screen engine, banks C and H are used for
    // backgrounds and banks D and I are used for sprites. Nitro Engine only
    // uses bank E for palettes, so the only thing we need to do is to tell
    // Nitro Engine to only use banks A and B and leave C and D unused.
    NE_Init3D();
    NE_TextureSystemReset(0, 0, NE_VRAM_AB);
    NE_ShadingEnable(true);

    // Load map and player
    camera = NE_CameraCreate();
    if (map.Load() == -1)
    {
        WaitLoop();
    }
    if (player.Load() == -1)
    {
        WaitLoop();
    }

    // Setup lighting
    NE_LightSet(0, NE_White, 0, -1, -1);

    // Setup background color
    NE_ClearColorSet(NE_Black, 31, 63);

    // Setup camera
    NE_CameraSet(camera,
                 0, 12.5, -4,
                 0, 1, 8.5,
                 0, 1, 0);

    // Move player to starting position
    player.Translate(camera, -11.5, 0, 4.5);
    player.targetX = 4;
    player.tileX = 4;
    player.targetZ = 1;
    player.tileZ = 1;
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

    // Set dialogue -- DEBUG
    SetDialogue(GALE, "M-M-Mister White!", frame);
}

void Game::SetDialogue(Speaker speaker, char line[], int startFrame)
{
    if (mode == DIALOGUE)
    {
        return;
    }
    mode = DIALOGUE;

    // Set dialog params
    currentSpeaker = speaker;
    currentLineStartFrame = startFrame;
    currentSpeakerAnimation = 0;
    
    // copy line to currentLine
    for (int i = 0; i < 128; i++)
    {
        currentLine[i] = line[i];
    }

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
    NF_CreateSprite(1, currentSpeaker + 10, 0, 0, 64, 27);
    NF_EnableSpriteRotScale(1, currentSpeaker + 10, currentSpeaker + 10, true);
    NF_SpriteRotScale(1, currentSpeaker + 10, 0, 386, 386);
}

void Game::UpdateDialogue(volatile int frame)
{
    uint charsToPrint = (frame - currentLineStartFrame) / 4;
    if (charsToPrint > strlen(currentLine))
    {
        charsToPrint = strlen(currentLine);
    }
    char lineToPrint[charsToPrint];
    strncpy(lineToPrint, currentLine, charsToPrint);
    lineToPrint[charsToPrint] = '\0';
    NF_WriteText(1, 0, 1, 21, lineToPrint);

    // Update speaker animation
    if ((frame - currentLineStartFrame) % 15 == 0)
    {
        currentSpeakerAnimation++;
        if (currentSpeakerAnimation > 7)
        {
            currentSpeakerAnimation = 0;
        }
        NF_SpriteFrame(1, currentSpeaker + 10, currentSpeakerAnimation);
    }
}

void Game::ClearDialogue()
{
    if (mode != DIALOGUE)
    {
        return;
    }

    NF_DeleteTiledBg(1, 3);

    mode = MOVE;
}

void Game::Render(volatile int frame)
{
    // Enable  fog
    NE_FogEnable(3, NE_Red, 31, 3, 0x7c00);

    NE_CameraUse(camera);
    NE_PolyFormat(31, 0, NE_LIGHT_0, NE_CULL_NONE, NE_FOG_ENABLE);
    player.Update(camera, frame);
    player.Move(map);

    map.UpdateCamera(player.x, player.z);
    map.Draw();
    player.Draw();
}

void Game::Update(volatile int frame)
{
    // Wait for the VBlank interrupt
    NE_WaitForVBL(NE_UPDATE_ANIMATIONS);

    // Copy shadow OAM copy to the OAM of the 2D sub engine
    oamUpdate(&oamSub);

    // Debug - print coords
    if (debugFlag)
    {
        player.PrintCoords(map);
    }

    // Draw dialogue
    if (mode == DIALOGUE)
    {
        UpdateDialogue(frame);
    }

    // Update text layers
    NF_UpdateTextLayers();

    // Handle input
    scanKeys();
    if (mode == MOVE)
    {
        player.HandleInput(keysHeld());
    }

    // Refresh shadow OAM copy
    NF_SpriteOamSet(1);
}