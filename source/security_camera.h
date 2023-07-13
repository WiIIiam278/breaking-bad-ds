#ifndef SECURITY_CAMERA_H
#define SECURITY_CAMERA_H

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <string>

#include <filesystem.h>
#include <nds.h>
#include <NEMain.h>

class SecurityCamera
{
private:
    NE_Model *model;
    NE_Material *material;

    const float x = -22.5;
    const float y = 7;
    const float z = 16;

public:
    SecurityCamera();

    int Load();
    void FacePlayer(float playerX, float playerZ);
    void Draw();
};

#endif  // SECURITY_CAMERA_H