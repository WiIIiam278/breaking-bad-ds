#include "map.h"

Map::Map()
{
}

int Map::Load()
{
    Model = NE_ModelCreate(NE_Static);
    Material = NE_MaterialCreate();

    // Load assets from the filesystem
    if (NE_ModelLoadStaticMeshFAT(Model, "model/superlab.dl") == 0)
    {
        consoleDemoInit();
        printf("Couldn't load map mesh...");
        return -1;
    }

    if (NE_MaterialTexLoadFAT(Material, NE_A1RGB5, 256, 256, NE_TEXGEN_TEXCOORD, "model/superlab_tex.bin") == 0)
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

Tile Map::GetTileAt(int x, int z)
{
    if (x < 0 || x > 9 || z < 0 || z > 8)
    {
        return VOID;
    }
    return Tiles[z][x];
}

void Map::UpdateCamera(float playerX, float playerZ)
{
    Camera.FacePlayer(playerX, playerZ);
}

void Map::Draw()
{
    // Draw the map
    NE_ModelDraw(Model);

    // Draw the security cameras
    for (int i = 0; i < 2; i++)
    {
        Camera.Draw();
    }
}
