#include <deque>
#include "drawingRoutines.h"
#include "essentialFunctions.h"
#include "geometricPrimitives.h"
#include "miscFunctions.h"
#include "miscPrimitives.h"
#include "triangleMathEssentials.h"
#include "clippingRoutines.h"
#include "vectorMathEssentials.h"
#include "normalMathEssentials.h"
#include "globalVariables.h"
#include "matrixMathEssentials.h"

Vector3D VectorIntersectPlane(const Vector3D& planePoint, const Vector3D& planeNormal, const Vector3D& lineStart, const Vector3D& lineEnd, float& tVal)
{
  /*//This is math that I do not understand
  Vector3D normalizedPlaneNormal = planeNormal;
  NormalizeVector(normalizedPlaneNormal);
  float planeDValue = -GetDotProduct(normalizedPlaneNormal, planePoint);
  float adValue = GetDotProduct(lineStart, normalizedPlaneNormal);
  float bdValue = GetDotProduct(lineEnd, normalizedPlaneNormal);
  tVal = (-planeDValue - adValue) / (bdValue - adValue);
  Vector3D lineStartToEnd = SubtractVector(lineEnd, lineStart);
  Vector3D lineIntersect = lineStartToEnd;
  MultiplyVectorScalar(lineIntersect, tVal);
  return AddVector(lineStart, lineIntersect);*/ 

		Vector3D plane_n = NormalizeVectorOP(planeNormal);
		float plane_d = -GetDotProduct(plane_n, planePoint);
		float ad = GetDotProduct(lineStart, plane_n);
		float bd = GetDotProduct(lineEnd, plane_n);
		tVal = (-plane_d - ad) / (bd - ad);
		Vector3D lineStartToEnd = SubtractVector(lineEnd, lineStart);
		Vector3D lineToIntersect = MultiplyVectorScalarOP(lineStartToEnd, tVal);
		return AddVector(lineStart, lineToIntersect);
}

int TriangleClipWithPlane(const Vector3D& planePoint, const Vector3D& planeNormal, Triangle& inputTriangle, Triangle& outputTriangle1, Triangle& outputTriangle2)
{
  auto GetDistance = [&](const Vector3D& point)
    {
      //This is the plane equation
      return (planeNormal.x * point.x + planeNormal.y * point.y + planeNormal.z * point.z - GetDotProduct(planeNormal, planePoint));
    };

  //These are containers for storing the points that are inside or outside of the plane to
  //ClipAgainst
  Vector3D* insidePoints[3]; 
  int totalInsidePoints = 0;
  Vector3D* outsidePoints[3];
  int totalOutsidePoints = 0;
  Vector2D* insideTextels[3];
  int totalInsideTextels = 0;
  Vector2D* outsideTextels[3];
  int totalOutsideTextels = 0;
  
		// Get signed distance of each point in triangle to plane
		float d0 = GetDistance(inputTriangle.points[0]);
		float d1 = GetDistance(inputTriangle.points[1]);
		float d2 = GetDistance(inputTriangle.points[2]);

		if (d0 >= 0) 
    { 
      insidePoints[totalInsidePoints++] = &inputTriangle.points[0];
      insideTextels[totalInsideTextels++] = &inputTriangle.texels[0]; 
    }
		else 
    {
			outsidePoints[totalOutsidePoints++] = &inputTriangle.points[0];
      outsideTextels[totalOutsideTextels++] = &inputTriangle.texels[0];
		}
		if (d1 >= 0) 
    {
			insidePoints[totalInsidePoints++] = &inputTriangle.points[1];
      insideTextels[totalInsideTextels++] = &inputTriangle.texels[1];
		}
		else 
    {
			outsidePoints[totalOutsidePoints++] = &inputTriangle.points[1];
      outsideTextels[totalOutsideTextels++] = &inputTriangle.texels[1];
		}
		if (d2 >= 0)
    {
			insidePoints[totalInsidePoints++] = &inputTriangle.points[2];
      insideTextels[totalInsideTextels++] = &inputTriangle.texels[2];
		}
		else 
    {
			outsidePoints[totalOutsidePoints++] = &inputTriangle.points[2];
      outsideTextels[totalOutsideTextels++] = &inputTriangle.texels[2];
		}
  
  //Now we must classify the points 
  
  //This means no points are valid. Reject the entire triangle
  if(totalInsidePoints == 0)
  {
    //The triangle has been clipped from existence
    //cout << "CASE 1\n";
    return 0;
  }

  //This means all points are valid. Pass the triangle as an output
  if(totalInsidePoints == 3)
  {
    outputTriangle1 = inputTriangle;
    //cout << "CASE 2\n";
    return 1;
  }

  //This means that 2 points lie outside the triangle
  //When the outside region is clipped, the triangle simply becomes smaller
  if(totalInsidePoints == 1 && totalOutsidePoints == 2)
  {
    //cout << "CASE 3\n";
    //Copy color information to smaller output Triangle
    if(SETTINGS_MAP[VISUALIZE_CLIPPING] == true)
      outputTriangle1.color = olc::BLUE;
    else
      outputTriangle1.color = inputTriangle.color;
    //We will now assign the output triangle 
    //The inside point is already valid
    outputTriangle1.points[0] = *insidePoints[0];
    outputTriangle1.texels[0] = *insideTextels[0];

    //The 2 remaining points are on the intersection between the plane and the triangle
    float tVal;
    outputTriangle1.points[1] = VectorIntersectPlane(planePoint, planeNormal, *insidePoints[0], *outsidePoints[0], tVal);
    outputTriangle1.texels[1].u = tVal * (outsideTextels[0]->u - insideTextels[0]->u) + insideTextels[0]->u; 
    outputTriangle1.texels[1].v = tVal * (outsideTextels[0]->v - insideTextels[0]->v) + insideTextels[0]->v;
    outputTriangle1.texels[1].w = tVal * (outsideTextels[0]->w - insideTextels[0]->w) + insideTextels[0]->w;

    outputTriangle1.points[2] = VectorIntersectPlane(planePoint, planeNormal, *insidePoints[0], *outsidePoints[1], tVal);
    outputTriangle1.texels[2].u = tVal * (outsideTextels[1]->u - insideTextels[0]->u) + insideTextels[0]->u; 
    outputTriangle1.texels[2].v = tVal * (outsideTextels[1]->v - insideTextels[0]->v) + insideTextels[0]->v;
    outputTriangle1.texels[2].w = tVal * (outsideTextels[1]->w - insideTextels[0]->w) + insideTextels[0]->w;

    return 1;
  }

  if(totalInsidePoints == 2 && totalOutsidePoints == 1)
  {
    //We will return 2 triangles
    //Copy the color information
    //cout << "CASE 4\n";
    if(SETTINGS_MAP[VISUALIZE_CLIPPING] == true)
      outputTriangle1.color = olc::GREEN;
    else
      outputTriangle1.color = inputTriangle.color;

    if(SETTINGS_MAP[VISUALIZE_CLIPPING] == true)
      outputTriangle2.color = olc::RED;
    else
      outputTriangle2.color = inputTriangle.color;
    
    float tVal;

    //First output triangle
    outputTriangle1.points[0] = *insidePoints[0];
    outputTriangle1.points[1] = *insidePoints[1];
    outputTriangle1.texels[0] = *insideTextels[0];
    outputTriangle1.texels[1] = *insideTextels[1];

    outputTriangle1.points[2] = VectorIntersectPlane(planePoint, planeNormal, *insidePoints[0], *outsidePoints[0], tVal);
    outputTriangle1.texels[2].u = tVal * (outsideTextels[0]->u - insideTextels[0]->u) + insideTextels[0]->u;
    outputTriangle1.texels[2].v = tVal * (outsideTextels[0]->v - insideTextels[0]->v) + insideTextels[0]->v;
    outputTriangle1.texels[2].w = tVal * (outsideTextels[0]->w - insideTextels[0]->w) + insideTextels[0]->w;

    //Second output triangle 
    outputTriangle2.points[0] = *insidePoints[1];
    outputTriangle2.points[1] = outputTriangle1.points[2];
    outputTriangle2.texels[0] = *insideTextels[1];
    //This used to be different, check backup of 8/11/2024
    outputTriangle2.texels[1] = outputTriangle1.texels[2];

    outputTriangle2.points[2] = VectorIntersectPlane(planePoint, planeNormal, *insidePoints[1], *outsidePoints[0], tVal);
	outputTriangle2.texels[2].u = tVal * (outsideTextels[0]->u - insideTextels[1]->u) + insideTextels[1]->u;
	outputTriangle2.texels[2].v = tVal * (outsideTextels[0]->v - insideTextels[1]->v) + insideTextels[1]->v;
	outputTriangle2.texels[2].w = tVal * (outsideTextels[0]->w - insideTextels[1]->w) + insideTextels[1]->w;
    return 2;
  }

  //To make LSP stop complaining
  return -1;
}

void DoScreenSpaceClipping(const RenderingInstance& RI, const Triangle& triangleToRaster, deque<Triangle>& triangleOutputs)
{
      Triangle clippedTriangles[2];
      deque<Triangle> trianglesQueue;
      trianglesQueue.push_back(triangleToRaster);
      int newTrianglesNumber = 1;
      for(int planeNumber = 0; planeNumber < 4; planeNumber++)
      {
        int trianglesToAdd = 0;
        while(newTrianglesNumber > 0)
        {
          Triangle triangleToTest = trianglesQueue.front();
          trianglesQueue.pop_front();
          newTrianglesNumber--;

          switch(planeNumber)
          {
            case(0):
              trianglesToAdd = TriangleClipWithPlane({0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f},triangleToTest, clippedTriangles[0], clippedTriangles[1]);
              //cout << trianglesToAdd << " resulted from case 1\n";
              break;
            case(1):
              trianglesToAdd = TriangleClipWithPlane({0.0f, (float)RI.engine->ScreenHeight() - 1, 0.0f}, {0.0f, -1.0f, 0.0f},triangleToTest, clippedTriangles[0], clippedTriangles[1]);
              //cout << trianglesToAdd << " resulted from case 2\n";
              break;
            case(2):
              trianglesToAdd = TriangleClipWithPlane({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},triangleToTest, clippedTriangles[0], clippedTriangles[1]);
              //cout << trianglesToAdd << " resulted from case 3\n";
              break;
            case(3):
              trianglesToAdd = TriangleClipWithPlane({(float)RI.engine->ScreenWidth() - 1, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},triangleToTest, clippedTriangles[0], clippedTriangles[1]);
              //cout << trianglesToAdd << " resulted from case 4\n";
              break;
          }

          for(int k = 0; k < trianglesToAdd; k++)
          {
            trianglesQueue.push_back(clippedTriangles[k]);
          }
        }
        newTrianglesNumber = trianglesQueue.size();
      }

      for(int j = 0; j < trianglesQueue.size(); j++)
      {
        triangleOutputs.push_back(trianglesQueue[j]);
      }
  //PrintTrianglesToDisk(rasterizedTriangles, ConcatenatePaths({GetPathFromResources(),"3D_ENGINE_REBORN_TRIANGLES.txt"}));
}

void DoViewSpaceClipping(olc::PixelGameEngine* engine, Player* player, vector<Triangle>& trianglesToRaster, vector<Vector3D>& normalsToRaster, Triangle& cameraTransformedTriangle)
{
  //Alias for readability and to make refactoring easier
  const float& VISION_NEAR = player->camera.GetFacingPlanes().first;
  const Matrix4x4& PROJECTION_MATRIX = player->camera.GetCameraProjectionMatrix();
  if(SETTINGS_MAP[DO_VIEW_SPACE_CLIPPING] == true)
  {
    //The below section will clip objects that are closer than VISION_NEAR
    int clippedTrianglesNumber = 0;
    Triangle clippedTriangles[2];

    clippedTrianglesNumber = TriangleClipWithPlane({0.0f, 0.0f, VISION_NEAR}, {0.0f, 0.0f, 1.0f}, cameraTransformedTriangle, clippedTriangles[0], clippedTriangles[1]);
    //cout << "Clipped Triangles Number: " << clippedTrianglesNumber << "\n\n";
    //We are now going from view space(relative to camera) to screen space
    for(int i = 0; i < clippedTrianglesNumber; i++)
    {
      Triangle projectedTriangle = MultiplyTriangle(clippedTriangles[i], PROJECTION_MATRIX, false);

      NormalizeTriangleTextels(projectedTriangle);

      //Aka Normalizing
      ConvertToDNCoordinates(projectedTriangle);
      
      //Fixing inverted axes
      InvertTriangleXY(projectedTriangle);
      ScreenNormalizeTriangle(projectedTriangle, (float)engine->ScreenWidth(), (float)engine->ScreenHeight());
      trianglesToRaster.push_back(projectedTriangle);
     
      //Normals section 
      Vector3D tempNormal = GetNormal(clippedTriangles[i]);
      Vector3D normalPoint = GetProjectedNormal(engine, PROJECTION_MATRIX, clippedTriangles[i], tempNormal);
      normalsToRaster.push_back(normalPoint);
    }
  } 
  
  if(SETTINGS_MAP[DO_VIEW_SPACE_CLIPPING] == false)
  {
    Triangle projectedTriangle = MultiplyTriangle(cameraTransformedTriangle, PROJECTION_MATRIX, false);
    NormalizeTriangleTextels(projectedTriangle);
    ConvertToDNCoordinates(projectedTriangle);
    //Fixing inverted axes
    InvertTriangleXY(projectedTriangle);

    ScreenNormalizeTriangle(projectedTriangle, (float)engine->ScreenWidth(), (float)engine->ScreenHeight());
    trianglesToRaster.push_back(projectedTriangle);
    if(SETTINGS_MAP[DRAW_NORMALS] == true)
    {
        Vector3D tempNormal = GetNormal(projectedTriangle);
        Vector3D normalPoint = GetProjectedNormal(engine, PROJECTION_MATRIX, projectedTriangle, tempNormal);
        normalsToRaster.push_back(normalPoint);
    }
  }
}
