#pragma once
#include <optional>
#include <vector>
#include "geometricPrimitives.h"
#include "inputManager.h"
#include "miscPrimitives.h"
#include "olcPixelGameEngine.h"

inline int currentObjectSet = 0;

void SortTriangles(vector<Triangle>& vecToSort);
//Returns the view matrix or look at matrix
Matrix4x4 DoInputLoop(olc::PixelGameEngine* engine, Player* player);
void ClearScreenPerformance(olc::PixelGameEngine* engine, const vector<Triangle>& trianglesToRaster);
float GetNoneMaterialLuminances(const Vector3D& normal, const deque<Light*>& lightsDeque);
olc::Pixel GetDiffuseMaterialColor(const Vector3D& normal, const olc::Pixel& diffuseColor, const deque<Light*>& lightsDeque);
NormalizedPixel GetPartiallyIlluminatedColorCode(const Vector3D& normal, const deque<Light*>& lightsDeque);
void DrawTriangleToScreen(const RenderingInstance& RI, const Triangle& triangleInput,
                          const Vector3D& illuminationNormal, const deque<Light*>& lightsDeque, 
                          const short& materialType, const olc::Decal* texture);
void DrawNormal(const RenderingInstance& RI, const Vector3D& normal, const Triangle& triangleSource);
void PopulateOLCPoints(const Triangle& inputTriangle, olc::vf2d& point1, olc::vf2d& point2, olc::vf2d& point3);
void DoAuxiliaryInputLoop(olc::PixelGameEngine* engine, MeshList& allObjects, deque<Light*>& allLights);
Vector3D GetProjectedNormal(olc::PixelGameEngine* engine, const Matrix4x4& projectionMatrix, const Triangle& rawTriangleInput, const Vector3D& normalInput);
void ClearAllObjectsandLights(MeshList& allObjects, deque<Light*>& allLights);
void SetInitialObjects(olc::PixelGameEngine* engine, MeshList& allObjects, deque<Light*>& allLights, 
                       std::optional<BASIC_CONTROLS_ENUM> sceneNumber = std::nullopt);
