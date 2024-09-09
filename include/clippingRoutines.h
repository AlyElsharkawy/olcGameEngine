#pragma once
#include <vector>
#include "geometricPrimitives.h"
#include "miscPrimitives.h"
#include "olcPixelGameEngine.h"

Vector3D VectorIntersectPlane(const Vector3D& planePoint, const Vector3D& planeNormal, const Vector3D& lineStart, const Vector3D& lineEnd);
int TriangleClipWithPlane(const Vector3D& planePoint, const Vector3D& planeNormal, Triangle& inputTriangle, Triangle& outputTriangle1, Triangle& outputTriangle2);
void DoScreenSpaceClipping(const RenderingInstance& RI ,const vector<Triangle>& trianglesToRaster, const Mesh& meshInput);
void DoViewSpaceClipping(olc::PixelGameEngine* engine, Player* player,vector<Triangle>& trianglesToRaster, const Vector3D& normal, Triangle& cameraTransformedTriangle);
