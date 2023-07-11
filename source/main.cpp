#include <stdio.h>
#include <stdlib.h>
#include <cmath>

#include <filesystem.h>
#include <nds.h>
#include <NEMain.h>
#include <nf_lib.h>

#include <string>

enum Tile
{
    VOID,
    FLOOR,
    WALL
};

enum Direction
{
    LEFT,
    DOWN,
    RIGHT,
    UP
};

class SecurityCamera
{
public:
    NE_Model *Model;
    NE_Material *Material;

    float x = -24;
    float y = 4.25;
    float z = 16;

    SecurityCamera(){};

    int Load(void)
    {
        Model = NE_ModelCreate(NE_Static);
        Material = NE_MaterialCreate();

        // Load assets from the filesystem
        if (NE_ModelLoadStaticMeshFAT(Model, "camera.dl") == 0)
        {
            consoleDemoInit();
            printf("Couldn't load security camera mesh...");
            return -1;
        }

        if (NE_MaterialTexLoadFAT(Material, NE_A1RGB5, 64, 64, NE_TEXGEN_TEXCOORD, "camera_tex.bin") == 0)
        {
            consoleDemoInit();
            printf("Couldn't load security camera textures...");
            return -1;
        }

        // Assign material to the model
        NE_ModelSetMaterial(Model, Material);

        // Set model rotation, position and scale
        int scale = 10250;
        NE_ModelScaleI(Model, scale, scale, scale);
        NE_ModelTranslate(Model, x, y, z);
        return 0;
    }

    void FacePlayer(float playerX, float playerZ) 
    {
        float angle = atan2(playerZ - z, playerX - x) * 180.0f / M_PI;
        if (angle > 90.0f) {
            angle = 90.0f;
        } else if (angle < -90.0f) {
            angle = -90.0f;
        }
        
        // Rotate the camera about the y-axis
        NE_ModelSetRot(Model, 0, -(angle + 90), 0);
    }

    void Draw()
    {
        // Draw the security camera
        NE_ModelDraw(Model);
    }
};

class Map
{
public:
    NE_Model *Model;
    NE_Material *Material;
    SecurityCamera Camera;

    // Z x X (top is closer to the camera)
    Tile Tiles[9][9] = {
        {FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR},
        {WALL,  FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR},
        {WALL,  FLOOR, FLOOR, WALL,  WALL,  WALL,  FLOOR, FLOOR, FLOOR},
        {WALL,  FLOOR, FLOOR, WALL,  WALL,  WALL,  WALL,  FLOOR, FLOOR},
        {WALL,  FLOOR, FLOOR, WALL,  FLOOR, FLOOR, WALL,  FLOOR, FLOOR},
        {WALL,  FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR},
        {WALL,  FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR},
        {WALL,  WALL,  FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR},
        {WALL,  WALL,  WALL,  WALL,  WALL,  WALL,  WALL,  WALL,  WALL}
    };

    Map(){};

    Tile GetTileAt(int x, int z)
    {
        if (x < 0 || x > 8 || z < 0 || z > 8)
        {
            return VOID;
        }
        return Tiles[z][x];
    }

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

        if (NE_MaterialTexLoadFAT(Material, NE_A1RGB5, 256, 256, NE_TEXGEN_TEXCOORD, "superlab_tex.bin") == 0)
        {
            consoleDemoInit();
            printf("Couldn't load map textures...");
            return -1;
        }

        // Set model rotation on the x axis
        NE_ModelSetRot(Model, 0, 3 * (512 / 4), 0);

        // Set scale
        int scale = 26600;
        NE_ModelScaleI(Model, scale, scale, scale);

        // Set position
        NE_ModelTranslate(Model, -13, 0, 10);

        // Assign material to the model
        NE_ModelSetMaterial(Model, Material);

        // Load security camera
        if (Camera.Load() != 0)
        {
            return -1;
        }

        return 0;
    }

    void UpdateCamera(float playerX, float playerZ)
    {
        Camera.FacePlayer(playerX, playerZ);
    }

    void Draw()
    {
        // Draw the map
        NE_ModelDraw(Model);

        // Draw the security cameras
        for (int i = 0; i < 2; i++)
        {
            Camera.Draw();
        }
    }
};

class Walter
{
public:
    NE_Model *Model;
    NE_Material *Material;

    Direction facing = DOWN; // 0 = left, 1 = down, 2 = right, 3 = up
    float rotation = (facing + 1) * (512 / 4);
    float x = 0;
    float y = 0.4;
    float z = 0;

    float speed = 0.125;
    bool walking = false;

    int tileX;
    int tileZ;
    int targetX;
    int targetZ;

    Walter(){};

    int Load(void)
    {
        Model = NE_ModelCreate(NE_Static);
        Material = NE_MaterialCreate();

        // Load assets from the filesystem
        if (NE_ModelLoadStaticMeshFAT(Model, "walter.dl") == 0)
        {
            consoleDemoInit();
            printf("Couldn't load walter mesh...");
            return -1;
        }

        if (NE_MaterialTexLoadFAT(Material, NE_A1RGB5, 128, 128, NE_TEXGEN_TEXCOORD, "walter_tex.bin") == 0)
        {
            consoleDemoInit();
            printf("Couldn't load walter textures...");
            return -1;
        }

        // Assign material to the model
        NE_ModelSetMaterial(Model, Material);

        // Set model rotation, position and scale
        int scale = 5500;
        NE_ModelSetRot(Model, 0, rotation, 0);
        NE_ModelScaleI(Model, scale, scale, scale);
        NE_ModelTranslate(Model, x, y, z);

        return 0;
    }

    void Draw()
    {
        NE_ModelDraw(Model);
    }

    void Move(Map map)
    {
        if (!walking)
        {
            return;
        }

        switch (facing)
        {
        case LEFT:
            targetX = tileX - 1;
            break;
        case DOWN:
            targetZ = tileZ - 1;
            break;
        case RIGHT:
            targetX = tileX + 1;
            break;
        case UP:
            targetZ = tileZ + 1;
            break;
        }

        Tile target = map.GetTileAt(targetX, targetZ);
        if (target != FLOOR)
        {
            targetX = tileX;
            targetZ = tileZ;
            walking = false;
        }
    }

    void Update(NE_Camera *camera, volatile int *frame)
    {
        // Update position
        float translateX = -1.3 + (-targetX * 2.6);
        float translateZ = 1.9 + (targetZ * 2.4);

        // Bob up and down
        float dY = (sin(*frame / 3.0) / 50.0) * 3;
        float allowance = 0.05;
        if (translateX > x + allowance)
        {
            Translate(camera, speed, dY, 0);
        }
        else if (translateX < x - allowance)
        {
            Translate(camera, -speed, dY, 0);
        }
        else if (translateZ > z + allowance)
        {
            Translate(camera, 0, dY, speed);
        }
        else if (translateZ < z - allowance)
        {
            Translate(camera, 0, dY, -speed);
        }
        else 
        {
            tileX = targetX;
            tileZ = targetZ;
        }

        // Update direction
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
        NE_CameraMove(camera, x, 0, 0);
        NE_ModelTranslate(Model, x, y, z);
    }

    Tile GetPlayerTile(Map map)
    {
        return map.GetTileAt(tileX, tileZ);
    }

    void PrintCoords(Map map)
    {
        char coords[100];
        sprintf(coords, "x: %.1f, y: %.1f, z: %.1f", x, y, z);
        NF_WriteText(1, 0, 1, 1, coords);

        char tileCoords[100];
        sprintf(tileCoords, "tileX: %i (%i), tileZ: %i (%i)", tileX, targetX, tileZ, targetZ);
        NF_WriteText(1, 0, 1, 2, tileCoords);

        char tileType[100];
        sprintf(tileType, "tileType: %i", GetPlayerTile(map));
        NF_WriteText(1, 0, 1, 3, tileType);

        NF_UpdateTextLayers();
    }
};

NE_Camera *camera;
Map map;
Walter player;

volatile int frame = 0;

void WaitLoop(void)
{
    while (1)
    {
        swiWaitForVBlank();
    }
}

void Draw3DScene(void)
{
    // Enable  fog
    NE_FogEnable(3, NE_Red, 31, 3, 0x7c00);

    NE_CameraUse(camera);
    NE_PolyFormat(31, 0, NE_LIGHT_0, NE_CULL_NONE, NE_FOG_ENABLE);
    player.Update(camera, &frame);
    player.Move(map);

    map.UpdateCamera(player.x, player.z);
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
                 0, 12.5, -3,
                 0, 1, 8.5,
                 0, 1, 0);
    
    // Move player to starting position
    player.Translate(camera, -11.5, 0, 4.5);
    player.targetX = 4;
    player.tileX = 4;
    player.targetZ = 1;
    player.tileZ = 1;
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
        player.PrintCoords(map);

        // Start processing a new frame after the 2D elements have been updated.
        scanKeys();
        uint32 keys = keysHeld();

        bool moving = player.targetX != player.tileX || player.targetZ != player.tileZ;
        player.walking = false;
        if (keys & KEY_LEFT && (!moving || player.facing == LEFT))
        {
            player.facing = LEFT;
            player.walking = true;
        }
        if (keys & KEY_DOWN && (!moving || player.facing == DOWN))
        {
            player.facing = DOWN;
            player.walking = true;
        }
        if (keys & KEY_RIGHT && (!moving || player.facing == RIGHT))
        {
            player.facing = RIGHT;
            player.walking = true;
        }
        if (keys & KEY_UP && (!moving || player.facing == UP))
        {
            player.facing = UP;
            player.walking = true;
        }

        // Refresh shadow OAM copy
        NF_SpriteOamSet(1);

        // Draw 3D scene
        NE_Process(Draw3DScene);
    }

    return 0;
}
