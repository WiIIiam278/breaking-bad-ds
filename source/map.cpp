#include "map.h"

Map::Map()
{
}

int Map::Load()
{
    model = NE_ModelCreate(NE_Static);
    material = NE_MaterialCreate();

    // Load assets from the filesystem
    if (NE_ModelLoadStaticMeshFAT(model, "model/superlab.dl") == 0)
    {
        consoleDemoInit();
        printf("Couldn't load map mesh...");
        return -1;
    }

    if (NE_MaterialTexLoadFAT(material, NE_A1RGB5, 256, 256, NE_TEXGEN_TEXCOORD, "model/superlab_tex.bin") == 0)
    {
        consoleDemoInit();
        printf("Couldn't load map textures...");
        return -1;
    }

    // Set model rotation on the x axis
    NE_ModelSetRot(model, 0, 3 * (512 / 4), 0);

    // Set scale
    int scale = 26600;
    NE_ModelScaleI(model, scale, scale, scale);

    // Set position
    NE_ModelTranslate(model, -13, 0, 10);

    // Assign material to the model
    NE_ModelSetMaterial(model, material);

    // Load security camera
    if (securityCamera.Load() != 0)
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

void Map::RotateSecurityCamera(float playerX, float playerZ)
{
    securityCamera.FacePlayer(playerX, playerZ);
}

void Map::Draw()
{
    // Draw the map
    NE_ModelDraw(model);

    // Draw the security cameras
    for (int i = 0; i < 2; i++)
    {
        securityCamera.Draw();
    }
}
