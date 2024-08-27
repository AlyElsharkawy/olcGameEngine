#pragma once
#include "geometricPrimitives.h"
Triangle MultiplyTriangle(const Triangle& input, const Matrix4x4& transformMatrix, bool normalize = false);
Triangle ScaleTriangle(const Triangle& input, float xScale = 1.0f, float yScale = 1.0f, float zScale = 1.0f);
Triangle RotateTriangle(const Triangle& input, const Matrix4x4& transformMatrix);
Triangle TranslateTriangle(const Triangle& input, float moveX, float moveY, float moveZ);
void TranslateTriangleIP(Triangle& input, float moveX, float moveY, float moveZ);
void ScreenNormalizeTriangle(Triangle& input, const float ScreenWidth, const float ScreenHeight);
void InvertTriangleXY(Triangle& input);
void NormalizeTriangleTextels(Triangle& input);
Vector3D GetMidpoint(const Triangle& input);
void ConvertToDNCoordinates(Triangle& input);

