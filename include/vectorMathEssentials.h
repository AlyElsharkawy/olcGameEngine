#pragma once
#include "geometricPrimitives.h"

void NormalizeVector(Vector3D& input);
Vector3D AddVector(const Vector3D& input1, const Vector3D& input2);
Vector2D AddVector(const Vector2D& input1, const Vector2D& input2);
Vector3D SubtractVector(const Vector3D& input1, const Vector3D& input2);
Vector2D SubtractVector(const Vector2D& input1, const Vector2D& input2);
Vector3D GetCrossProduct(const Vector3D& line1, const Vector3D& line2);
float GetDotProduct(const Vector3D& vectorA, const Vector3D& vectorB, const Vector3D& offset = {0,0,0});
void MultiplyVectorScalar(Vector3D& input, float scalarValue);
void MultiplyVectorScalar(Vector2D& input, float scalarValue);
void DivideVectorScalar(Vector3D& input, float scalarValue);
void DivideVectorScalar(Vector2D& input, float scalarValue);
float GetVectorLength(const Vector3D& input);
float GetDistanceBetweenPoints(const Vector3D& input1, const Vector3D& input2);
