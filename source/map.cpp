#include "map.h"

Map::Map()
{
}

int Map::Load()
{
    pipePos[0] = -191;
    pipePos[1] = -78;

    material = NE_MaterialCreate();
    if (NE_MaterialTexLoadFAT(material, NE_A1RGB5, 256, 256, NE_TEXGEN_TEXCOORD, "model/superlab_tex.bin") == 0)
    {
        consoleDemoInit();
        printf("Couldn't load map textures...");
        return -1;
    }
    
    NE_MaterialSetPropierties(material,
                RGB15(18, 18, 18), // Diffuse
                RGB15(22, 21, 20),    // Ambient
                RGB15(0, 0, 0),    // Specular
                RGB15(0, 0, 0),    // Emission
                false, false);     // Vertex color, use shininess table

    for (int i = 0; i < MODEL_COUNT; i++)
    {
        models[i] = NE_ModelCreate(NE_Static);
        if (NE_ModelLoadStaticMeshFAT(models[i], modelPaths[i]) == 0)
        {
            consoleDemoInit();
            printf("Couldn't load map model: %s...", modelPaths[i]);
            return -1;
        }

        // Set model rotation on the x axis
        NE_ModelSetRot(models[i], 0, 3 * (512 / 4), 0);

        // Set scale
        int scale = 26600;
        NE_ModelScaleI(models[i], scale, scale, scale);

        // Set position
        NE_ModelSetCoord(models[i], -13, 0, 10);

        // Assign material to the model
        NE_ModelSetMaterial(models[i], material);

        // Free mesh when deleted
        NE_ModelFreeMeshWhenDeleted(models[i]);
    }

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

void Map::AdjustPipe(float pipeX, float pipeZ)
{
    NE_ModelSetCoord(models[2], -13 - pipeX, -pipeZ, 10);
}

void Map::Draw(bool playerObscured)
{
    if (playerObscured && mixerAlpha > 20)
    {
        mixerAlpha--;
    }
    else if (!playerObscured && mixerAlpha < 31)
    {
        mixerAlpha++;
    }

    // Draw the map
    for (int i = 0; i < MODEL_COUNT; i++)
    {
        NE_PolyFormat(i > 0 ? mixerAlpha : 31, 0, NE_LIGHT_0, NE_CULL_NONE, NE_FOG_ENABLE);
        NE_ModelDraw(models[i]);
    }

    // Draw the security camera
    securityCamera.Draw();
}

void Map::Unload()
{
    // Unload the security cameras
    securityCamera.Unload();

    // Delete models
    for (int i = 0; i < MODEL_COUNT; i++)
    {
        NE_ModelDelete(models[i]);
    }

    // Unload the map
    NE_MaterialDelete(material);
}