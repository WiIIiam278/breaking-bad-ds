#include <stdio.h>
#include <stdlib.h>

#include <filesystem.h>
#include <nds.h>
#include <NEMain.h>
#include <nf_lib.h>

#include <string>

class Walter
{
public:
    NE_Model *Model;
    NE_Animation *Animation;
    NE_Material *Material;
    int facing = 1; // 0 = left, 1 = down, 2 = right, 3 = up
    float rotation = (facing + 1) * (512 / 4);
    bool walking = false;

    float x = 0;
    float y = 0;
    float z = 0;

    Walter(){};

    int Load(void)
    {
        Model = NE_ModelCreate(NE_Animated);
        Material = NE_MaterialCreate();
        Animation = NE_AnimationCreate();

        // Load assets from the filesystem
        if (NE_ModelLoadDSMFAT(Model, "walter.dsm") == 0)
        {
            consoleDemoInit();
            printf("Couldn't load model...");
            return -1;
        }

        if (NE_AnimationLoadFAT(Animation, "walter_idle1.dsa") == 0)
        {
            consoleDemoInit();
            printf("Couldn't load animation...");
            return -1;
        }

        if (NE_MaterialTexLoadFAT(Material, NE_A1RGB5, 128, 128, NE_TEXGEN_TEXCOORD,
                                  "walter_tex.bin") == 0)
        {
            consoleDemoInit();
            printf("Couldn't load texture...");
            return -1;
        }

        // Set model rotation on the x axis
        NE_ModelSetRot(Model, 0, rotation, 0);

        // Set position
        NE_ModelTranslate(Model, 1.3, 0, -7.2);

        // Assign material to the model
        // NE_MaterialSetPropierties(Material, NE_Black, NE_Black, NE_Black, NE_Black, false, true);
        NE_ModelSetMaterial(Model, Material);

        // Assign animation to the model and start it
        NE_ModelSetAnimation(Model, Animation);
        NE_ModelAnimStart(Model, NE_ANIM_LOOP, floattof32(0.1));
        return 0;
    }

    void Draw()
    {
        NE_ModelDraw(Model);
    };

    void Move(NE_Camera *camera)
    {
        if (!walking)
        {
            return;
        }

        float speed = 0.1;
        switch (facing)
        {
        case 0:
            Translate(camera, speed, 0, 0);
            break;
        case 1:
            Translate(camera, 0, 0, -speed);
            break;
        case 2:
            Translate(camera, -speed, 0, 0);
            break;
        case 3:
            Translate(camera, 0, 0, speed);
            break;
        }
    };

    void UpdateDirection()
    {
        int turningSpeed = 20;
        float target = (facing + 1) * (511 / 4);
        float changeBy = abs(target - rotation) > 10 ? (target - rotation > 0 ? turningSpeed : -turningSpeed) : 0;
        rotation += changeBy;
        NE_ModelSetRot(Model, 0, rotation, 0);
    }

    void Translate(NE_Camera *camera, float x, float y, float z)
    {
        this->x += x;
        this->y += y;
        this->z += z;
        NE_CameraMove(camera, x, y, z);
        NE_ModelTranslate(Model, x, y, z);
    };

    void PrintCoords()
    {
        char coords[100];
        sprintf(coords, "x: %f, z: %f", x, z);
        NF_WriteText(1, 0, 1, 1, coords);
        NF_UpdateTextLayers();
    }
};

class Map
{
public:
    NE_Model *Model;
    NE_Material *Material;

    Map() {};

    int Load(void)
    {
        Model = NE_ModelCreate(NE_Static);
        Material = NE_MaterialCreate();

        
        // Load assets from the filesystem
        if (NE_ModelLoadStaticMeshFAT(Model, "superlab.dl") == 0)
        {
            consoleDemoInit();
            printf("Couldn't load map mesh...");
            return -1;
        }

        if (NE_MaterialTexLoadFAT(Material, NE_A1RGB5, 256, 256, NE_TEXGEN_TEXCOORD,
                                  "superlab_tex.bin") == 0)
        {
            consoleDemoInit();
            printf("Couldn't load map textures...");
            return -1;
        }

        // Set model rotation on the x axis
        NE_ModelSetRot(Model, 0, 3 * (512 / 4), 0);
        
        // Set scale - The model is scaled 5.9x using obj2dl from the blender waveform
        int scale = 26600;
        NE_ModelScaleI(Model, scale, scale, scale);
        
        // Set position
        NE_ModelTranslate(Model, 0, 3.85, 0);

        // Assign material to the model
        NE_ModelSetMaterial(Model, Material);
        return 0;
    }

    void Draw()
    {
        NE_ModelDraw(Model);
    };
};

NE_Camera *camera;
Map map;
Walter player;

volatile int frame = 0;
bool flashingRed = false;

void WaitLoop(void)
{
    while (1)
    {
        swiWaitForVBlank();
    }
}

void Draw3DScene(void)
{
    player.Move(camera);
    NE_CameraUse(camera);
    NE_PolyFormat(31, 0, NE_LIGHT_0, NE_CULL_NONE, NE_FOG_DISABLE);

    player.UpdateDirection();

    map.Draw();
    player.Draw();
}

void LoadAndSetupGraphics3D(void)
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
                 1.3, 11.5, -14.2,
                 1.3, 2, -3,
                 0, 1, 0);
}

void LoadAndSetupGraphics2D(void)
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
    NF_LoadTextFont("fnt/default", "normal", 256, 256, 0);

    // Create text layer
    NF_CreateTextLayer(1, 0, 0, "normal");
}

void TickLights(void)
{
    if (frame % 120 == 0)
    {
        flashingRed = !flashingRed;

        if (flashingRed) {
            NE_LightSetColor(0, NE_Red);
        } else {
            NE_LightSetColor(0, NE_White);
        }
    }
}

int main(void)
{
    // Initialize nitroFS before doing anything else
    NF_Set2D(0, 0);
    NF_Set2D(1, 0);
    consoleDemoInit();
    printf("Starting nitroFS...\n");
    if (!nitroFSInit(NULL))
    {
        printf("Failed to start nitroFS\n");
        printf("Press START to exit\n");

        while (1)
        {
            swiWaitForVBlank();
            scanKeys();
            if (keysHeld() & KEY_START)
                return -1;
        }
    }
    swiWaitForVBlank();

    // Set the root folder to the nitroFS filesystem
    NF_SetRootFolder("NITROFS");

    // Setup interrupt handlers
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    // Setup 3D scene
    LoadAndSetupGraphics3D();
    LoadAndSetupGraphics2D();

    while (1)
    {
        // Update frame counter
        frame++;

        // Wait for the VBlank interrupt
        NE_WaitForVBL(NE_UPDATE_ANIMATIONS);

        // Copy shadow OAM copy to the OAM of the 2D sub engine
        oamUpdate(&oamSub);

        // Debug - print coords
        player.PrintCoords();

        // Start processing a new frame after the 2D elements have been updated.
        scanKeys();
        uint32 keys = keysHeld();

        player.walking = false;
        if (keys & KEY_LEFT)
        {
            player.facing = 0;
            player.walking = true;
        }
        if (keys & KEY_DOWN)
        {
            player.facing = 1;
            player.walking = true;
        }
        if (keys & KEY_RIGHT)
        {
            player.facing = 2;
            player.walking = true;
        }
        if (keys & KEY_UP)
        {
            player.facing = 3;
            player.walking = true;
        }

        // Tick lights
        TickLights();

        // Refresh shadow OAM copy
        NF_SpriteOamSet(1);

        // Draw 3D scene
        NE_Process(Draw3DScene);
    }

    return 0;
}
