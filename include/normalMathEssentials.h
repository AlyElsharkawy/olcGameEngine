#pragma once
#include "geometricPrimitives.h"

Vector3D GetNormal(const Triangle& input, bool normalize = true);
void ScreenNormalizeNormal(Vector3D& input, float ScreenWidth, float ScreenHeight);
Vector3D ScaleNormal(const Vector3D& source, const Vector3D& normal, const float& length);
