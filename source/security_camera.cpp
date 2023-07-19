#include "security_camera.h"

SecurityCamera::SecurityCamera()
{
}

int SecurityCamera::Load()
{
    model = NE_ModelCreate(NE_Static);
    material = NE_MaterialCreate();

    // Load assets from the filesystem
    if (NE_ModelLoadStaticMeshFAT(model, "model/camera.dl") == 0)
    {
        consoleDemoInit();
        printf("Couldn't load security camera mesh...");
        return -1;
    }

    if (NE_MaterialTexLoadFAT(material, NE_A1RGB5, 64, 64, NE_TEXGEN_TEXCOORD, "model/camera_tex.bin") == 0)
    {
        consoleDemoInit();
        printf("Couldn't load security camera textures...");
        return -1;
    }

    // Assign material to the model
    NE_ModelSetMaterial(model, material);

    // Set model rotation, position and scale
    int scale = 10250;
    NE_ModelScaleI(model, scale, scale, scale);
    NE_ModelTranslate(model, x, y, z);

    return 0;
}

void SecurityCamera::FacePlayer(float playerX, float playerZ)
{
    float angle = atan2(playerZ - z, playerX - x) * 180.0f / M_PI;
    if (angle > 90.0f)
    {
        angle = 90.0f;
    }
    else if (angle < -90.0f)
    {
        angle = -90.0f;
    }

    // Rotate the camera about the y-axis
    NE_ModelSetRot(model, 0, -(angle + 90), 0);
}

void SecurityCamera::Draw()
{
    // Draw the security camera
    NE_ModelDraw(model);
}

void SecurityCamera::Unload()
{
    // Free the model and material
    NE_MaterialDelete(material);
    NE_ModelDelete(model);
}