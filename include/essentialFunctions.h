#pragma once
#include <vector>
#include "geometricPrimitives.h"
#include "miscPrimitives.h"
#include "olcPixelGameEngine.h"

void SortTriangles(vector<Triangle>& vecToSort);
void DrawTexturedTriangle(const RenderingInstance& RI, const Triangle& input, const olc::Sprite* texture);
//Returns the view matrix or look at matrix
Matrix4x4 DoInputLoop(olc::PixelGameEngine* engine, const float& fElapsedTime, float& fYaw, float& fPitch, Vector3D& CAMERA, Vector3D& LookDirection);
void ClearScreenPerformance(olc::PixelGameEngine* engine, const vector<Triangle>& trianglesToRaster, const vector<Vector3D>& normalsToRaster);
float GetNoneMaterialLuminances(const Vector3D& normal, const deque<Light>& lightsDeque);
olc::Pixel GetDiffuseMaterialColor(const Vector3D& normal, const olc::Pixel& diffuseColor, const deque<Light>& lightsDeque);
void DrawTriangleToScreen(const RenderingInstance& RI, const Triangle& triangleInput, const short& materialType, const olc::Decal* texture);
void PopulateOLCPoints(const Triangle& inputTriangle, olc::vf2d& point1, olc::vf2d& point2, olc::vf2d& point3);
