#include "logo.h"

Logo::Logo()
{
}

int Logo::Load()
{
    backdrop = NE_ModelCreate(NE_Static);
    backdropMaterial = NE_MaterialCreate();

    model = NE_ModelCreate(NE_Static);
    material = NE_MaterialCreate();

    textPlane = NE_ModelCreate(NE_Static);
    textMaterial = NE_MaterialCreate();

    // Load assets from the filesystem
    if (NE_ModelLoadStaticMeshFAT(backdrop, "model/backdrop.dl") == 0
        || NE_ModelLoadStaticMeshFAT(model, "model/logo.dl") == 0
        || NE_ModelLoadStaticMeshFAT(textPlane, "model/logo_text.dl") == 0)
    {
        consoleDemoInit();
        printf("Couldn't load logo mesh/text plane...");
        return -1;
    }
    if (NE_MaterialTexLoadFAT(backdropMaterial, NE_A1RGB5, 128, 128, NE_TEXGEN_TEXCOORD, "model/backdrop_tex.bin") == 0
        || NE_MaterialTexLoadFAT(material, NE_A1RGB5, 128, 128, NE_TEXGEN_TEXCOORD, "model/logo_tex.bin") == 0
        || NE_MaterialTexLoadFAT(textMaterial, NE_A1RGB5, 128, 128, NE_TEXGEN_TEXCOORD, "model/logo_text_tex.bin") == 0)
    {
        consoleDemoInit();
        printf("Couldn't load logo/text textures...");
        return -1;
    }

    // Assign material to the model
    NE_ModelSetMaterial(backdrop, backdropMaterial);
    NE_ModelSetMaterial(model, material);
    NE_ModelSetMaterial(textPlane, textMaterial);

    // Set model rotation, position and scale
    int scale = 8250;
    NE_ModelScaleI(backdrop, scale * 3, scale * 3, scale * 3);
    NE_ModelScaleI(model, scale, scale, scale);
    NE_ModelScaleI(textPlane, scale, scale, scale);
    NE_ModelTranslate(backdrop, x + 15, y - 10, z + 5);
    NE_ModelTranslate(model, x, y, z);
    NE_ModelTranslate(textPlane, textX, y + 0.1, z + 4.55);

    NF_LoadSpriteGfx("sprite/start", 1, 64, 64);
    NF_LoadSpritePal("sprite/start", 1);
    NF_VramSpriteGfx(1, 1, 0, false); // This keeps unused frames in RAM
    NF_VramSpritePal(1, 1, 0);

    // Create, position & scale sprite
    NF_CreateSprite(1, START_SPRITE, 0, 0, 64, 32);
    NF_EnableSpriteRotScale(1, START_SPRITE, START_SPRITE, true);
    NF_SpriteRotScale(1, START_SPRITE, 0, 283, 283);
    
    return 0;
}

void Logo::Update(volatile int frame)
{
    if (x < TARGET_X)
    {
        x += X_SPEED;
        NE_ModelTranslate(model, X_SPEED, 0, 0);
    }
    if (textX < TARGET_TEXT_X)
    {
        textX += X_SPEED;
        NE_ModelTranslate(textPlane, X_SPEED, 0, 0);
    }

    NF_SpriteFrame(1, START_SPRITE, (frame > 345) ? (frame > 690) ? 2 : 1 : 0);

    if (frame > 690 && frame % 30 == 0)
    {
        showStartSprite = !showStartSprite;
        NF_ShowSprite(1, START_SPRITE, showStartSprite);
    }
}

void Logo::Draw(volatile int frame)
{
    NE_ModelDraw(backdrop);
    NE_ModelDraw(model);
    NE_ModelDraw(textPlane);
}

void Logo::Unload()
{
    // Free the model and material
    NE_ModelDelete(model);
    NE_ModelDelete(textPlane);
    NE_ModelDelete(backdrop);

    NE_MaterialDelete(material);
    NE_MaterialDelete(textMaterial);
    NE_MaterialDelete(backdropMaterial);

    // Unload sprite
    NF_UnloadSpriteGfx(1);
    NF_UnloadSpritePal(1);
    NF_FreeSpriteGfx(1, 0);
}