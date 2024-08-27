#pragma once
#include "geometricPrimitives.h"

Vector3D GetNormal(const Triangle& input, bool normalize = true);
void ScreenNormalizeNormal(Vector3D& input, float ScreenWidth, float ScreenHeight);

