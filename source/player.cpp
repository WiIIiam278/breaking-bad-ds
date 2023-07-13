#include "player.h"

Player::Player()
{
    // Player initialization code
}

int Player::Load()
{
    Model = NE_ModelCreate(NE_Static);
    Material = NE_MaterialCreate();

    // Load assets from the filesystem
    if (NE_ModelLoadStaticMeshFAT(Model, "model/walter.dl") == 0)
    {
        consoleDemoInit();
        printf("Couldn't load walter mesh...");
        return -1;
    }

    if (NE_MaterialTexLoadFAT(Material, NE_A1RGB5, 128, 128, NE_TEXGEN_TEXCOORD, "model/walter_tex.bin") == 0)
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

void Player::Move(Map &map)
{
    if (!walking)
        return;

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

void Player::Update(NE_Camera *camera, volatile int frame)
{
    // Update position
    float translateX = -1.3 + (-targetX * 2.6);
    float translateZ = 1.9 + (targetZ * 2.4);

    // Bob up and down as the player walks
    float dY = (sin(frame / 3.0) / 50.0) * 3;
    if (y + dY > 0.8 || y + dY < 0.4)
    {
        dY = -dY;
    }

    if (translateX > x + TILE_TOLERANCE)
    {
        Translate(camera, SPEED, dY, 0);
    }
    else if (translateX < x - TILE_TOLERANCE)
    {
        Translate(camera, -SPEED, dY, 0);
    }
    else if (translateZ > z + TILE_TOLERANCE)
    {
        Translate(camera, 0, dY, SPEED);
    }
    else if (translateZ < z - TILE_TOLERANCE)
    {
        Translate(camera, 0, dY, -SPEED);
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

void Player::Translate(NE_Camera *camera, float x, float y, float z)
{
    this->x += x;
    this->y += y;
    this->z += z;
    NE_ModelTranslate(Model, x, y, z);
    NE_CameraMove(camera, x, z / 6, z / 6);
}

Tile Player::GetPlayerTile(Map &map)
{
    return map.GetTileAt(tileX, tileZ);
}

void Player::PrintCoords(Map &map)
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

void Player::Draw()
{
    NE_ModelDraw(Model);
}