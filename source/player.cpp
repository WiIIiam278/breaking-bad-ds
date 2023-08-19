#include "player.h"

Player::Player()
{
}

int Player::Load(Character character, NE_Animation *animations[2], bool airJordans)
{
    this->character = character;
    this->lyingDown = false;
    this->maxSpeed = airJordans ? 0.155f : 0.125f;

    model = NE_ModelCreate(NE_Animated);
    material = NE_MaterialCreate();
    animation[0] = animations[0];
    animation[1] = animations[1];

    // Load model
    if (NE_ModelLoadDSMFAT(model, "model/player.dsm") == 0)
    {
        consoleDemoInit();
        printf("Couldn't load player mesh...");
        return -1;
    }

    // Load textures
    int texturesLoaded = 0;
    switch (character)
    {
    case CHAR_WALT:
        texturesLoaded = NE_MaterialTexLoadFAT(material, NE_A1RGB5, 128, 128,
                                            NE_TEXGEN_TEXCOORD, "model/walter_tex.bin");
        break;
    case CHAR_JESSE:
        texturesLoaded = NE_MaterialTexLoadFAT(material, NE_A1RGB5, 128, 128,
                                            NE_TEXGEN_TEXCOORD, "model/jessie_tex.bin");
        break;
    case CHAR_YEPPERS:
        texturesLoaded = NE_MaterialTexLoadFAT(material, NE_A1RGB5, 128, 128,
                                            NE_TEXGEN_TEXCOORD, "model/yeppers_tex.bin");
        break;
    }

    if (texturesLoaded == 0)
    {
        consoleDemoInit();
        printf("Couldn't load player textures...");
        return -1;
    }

    // Assign material to the model
    NE_ModelSetMaterial(model, material);

    // Set animation
    NE_ModelSetAnimation(model, animation[0]);
    NE_ModelSetAnimationSecondary(model, animation[1]);
    NE_ModelAnimStart(model, NE_ANIM_LOOP, floattof32(0.5));
    NE_ModelAnimSecondaryStart(model, NE_ANIM_LOOP, floattof32(1 + (airJordans ? 0.5 : 0)));

    // Set some propierties to the material
    NE_MaterialSetPropierties(material,
                              RGB15(20, 20, 20), // Diffuse
                              RGB15(11, 10, 9),  // Ambient
                              RGB15(0, 0, 0),    // Specular
                              RGB15(0, 0, 0),    // Emission
                              false, false);     // Vertex color, use shininess table
    
    // Free mesh when deleted
    NE_ModelFreeMeshWhenDeleted(model);

    return 0;
}

void Player::ResetPosition()
{
    // Set position, rotation and scale
    x = 0;
    y = 0.4;
    z = 0;
    rotation = (facing + 1) * (512 / 4);
    int scale = 5550;
    NE_ModelSetRot(model, 0, rotation, 0);
    NE_ModelScaleI(model, scale, scale, scale);
    NE_ModelSetCoordI(model, x, y, z);
}

void Player::Move(Map &map)
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
    if (target == WALL || target == VOID)
    {
        setRumble(rand() % 2 == 0);
        targetX = tileX;
        targetZ = tileZ;
        walking = false;
        return;
    }
}

void Player::Update(volatile int frame)
{
    // Update direction
    int turningSpeed = 20;
    float target = (facing + 1) * (511 / 4);
    float changeBy = abs(target - rotation) > 10 ? (target - rotation > 0 ? turningSpeed : -turningSpeed) : 0;
    rotation += changeBy;
    NE_ModelSetRot(model, lyingDown ? 90 : 0, rotation, 0);
    NE_ModelAnimSecondarySetFactor(model, floattof32((currentSpeed[0] + currentSpeed[1]) / maxSpeed));

    // Don't move walter if he's dead
    if (lyingDown)
    {
        lyingDownFrames++;
        currentSpeed[0] = currentSpeed[1] = 0;
        return;
    }

    // Don't move if in dialogue
    if (inDialogue)
    {
        targetX = tileX;
        targetZ = tileZ;
        walking = false;
        currentSpeed[0] = currentSpeed[1] = 0;
        return;
    }

    // Update position
    float translateX = -1.3 + (-targetX * 2.6);
    float translateZ = 1.9 + (targetZ * 2.4);

    // Bob up and down as the player walks
    float dY = (sin(frame / 3.0) / 50.0) * 3;
    if (y + dY > 0.8 || y + dY < 0.4)
    {
        dY = -dY;
    }

    for (int i = 0; i < 2; i++)
    {
        if (currentSpeed[i] > maxSpeed)
        {
            currentSpeed[i] = maxSpeed;
        }
    }

    if (translateX > x + TILE_SIZE)
    {
        currentSpeed[0] += (maxSpeed / ACCELERATION);
        currentSpeed[1] = 0;
        Translate(currentSpeed[0], dY, 0);
    }
    else if (translateX < x - TILE_SIZE)
    {
        currentSpeed[0] += (maxSpeed / ACCELERATION);
        currentSpeed[1] = 0;
        Translate(-currentSpeed[0], dY, 0);
    }
    else if (translateZ > z + TILE_SIZE)
    {
        currentSpeed[1] += (maxSpeed / ACCELERATION);
        currentSpeed[0] = 0;
        Translate(0, dY, currentSpeed[1]);
    }
    else if (translateZ < z - TILE_SIZE)
    {
        currentSpeed[1] += (maxSpeed / ACCELERATION);
        currentSpeed[0] = 0;
        Translate(0, dY, -currentSpeed[1]);
    }
    else
    {
        tileX = targetX;
        tileZ = targetZ;
        if (!walking)
        {
            currentSpeed[0] = currentSpeed[1] = 0;
        }
    }
}

void Player::Translate(float x, float y, float z)
{
    this->x += x;
    this->y += y;
    this->z += z;
    NE_ModelTranslate(model, x, y, z);
}

Tile Player::GetTile(Map &map)
{
    return map.GetTileAt(tileX, tileZ);
}

void Player::SetLyingDown(bool lyingDown)
{
    this->lyingDown = lyingDown;
    Translate(0, 0.3f, -2.0f);
    facing = DOWN;
    NE_ModelSetRot(model, lyingDown ? 90 : 0, rotation, 0);
}

void Player::HandleInput(uint32 keys)
{
    bool moving = targetX != tileX || targetZ != tileZ;
    walking = false;
    if (keys & KEY_LEFT && (!moving || facing == LEFT))
    {
        facing = LEFT;
        walking = true;
    }
    if (keys & KEY_DOWN && (!moving || facing == DOWN))
    {
        facing = DOWN;
        walking = true;
    }
    if (keys & KEY_RIGHT && (!moving || facing == RIGHT))
    {
        facing = RIGHT;
        walking = true;
    }
    if (keys & KEY_UP && (!moving || facing == UP))
    {
        facing = UP;
        walking = true;
    }
}

void Player::DrawShadow(float radius, u32 color)
{
    // Set the number of points to create the circle approximation
    const int numPoints = 12;

    // Calculate the angle between each point on the circle
    const float angleIncrement = 2.0f * 3.14159265359f / numPoints;

    // Calculate the positions of points forming the circle
    float circlePoints[numPoints][3];
    for (int i = 0; i < numPoints; ++i)
    {
        float angle = i * angleIncrement;
        circlePoints[i][0] = radius * cos(angle);
        circlePoints[i][1] = 0.6;
        circlePoints[i][2] = radius * sin(angle);
    }

    // Draw the triangles that form the circle
    float offset[3] = {x, 0, z + (lyingDown ? 1.5f : 0)};
    for (int i = 1; i < numPoints - 1; i++)
    {
        float *vertices[3] = {circlePoints[0], circlePoints[i], circlePoints[i + 1]};
        DrawTriangle(vertices, offset, color);
    }
}

void Player::DrawTriangle(float *vertices[], float offset[3], u32 color)
{
    NE_PolyBegin(GL_TRIANGLE);
    NE_PolyColor(color);
    glTranslatef(offset[0], offset[1], offset[2]);
    for (int i = 0; i < 3; ++i)
    {
        v16 x = floattov16(vertices[i][0]);
        v16 y = floattov16(vertices[i][1]);
        v16 z = floattov16(vertices[i][2]);
        NE_PolyVertexI(x, y, z);
    }
    glTranslatef(-offset[0], -offset[1], -offset[2]);
}

void Player::Draw(u32 outline)
{
    NE_PolyFormat(lyingDown ? 26 : 16, 8, NE_LIGHT_0, NE_CULL_NONE, NE_FOG_ENABLE);
    DrawShadow(lyingDown ? 0.85f + (float)std::min(0.85f, (float)(lyingDownFrames / 500.0f)) : 0.85f, lyingDown ? NE_Red : NE_Black);
    NE_PolyFormat(31, (8 + (outline * 8)), NE_LIGHT_0, NE_CULL_NONE, (NE_OtherFormatEnum)(NE_TOON_HIGHLIGHT_SHADING | NE_FOG_ENABLE));
    NE_ModelDraw(model);
}

void Player::Unload()
{
    NE_ModelDelete(model);
    NE_MaterialDelete(material);
}