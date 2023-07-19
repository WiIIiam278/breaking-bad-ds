#include "logo.h"

Logo::Logo()
{
}

int Logo::Load()
{
    model = NE_ModelCreate(NE_Static);
    material = NE_MaterialCreate();

    // Load assets from the filesystem
    if (NE_ModelLoadStaticMeshFAT(model, "model/logo.dl") == 0)
    {
        consoleDemoInit();
        printf("Couldn't load logo mesh...");
        return -1;
    }

    if (NE_MaterialTexLoadFAT(material, NE_A1RGB5, 128, 128, NE_TEXGEN_TEXCOORD, "model/logo_tex.bin") == 0)
    {
        consoleDemoInit();
        printf("Couldn't load logo textures...");
        return -1;
    }

    // Assign material to the model
    NE_ModelSetMaterial(model, material);

    // Set model rotation, position and scale
    int scale = 8250;
    NE_ModelScaleI(model, scale, scale, scale);
    NE_ModelTranslate(model, x, y, z);

    return 0;
}

void Logo::Update(volatile int frame)
{
    if (x < 0)
    {
        x += X_SPEED;
        NE_ModelTranslate(model, X_SPEED, 0, 0);
    }
}

void Logo::Draw()
{
    // Draw the security camera
    NE_ModelDraw(model);
}

void Logo::Unload()
{
    // Free the model and material
    NE_MaterialDelete(material);
    NE_ModelDelete(model);
}