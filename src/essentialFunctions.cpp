#include <vector>
#include <algorithm>
#include "matrixMathEssentials.h"
#include "essentialFunctions.h"
#include "geometricPrimitives.h"
#include "miscPrimitives.h"
#include "vectorMathEssentials.h"
#include "globalVariables.h"
#include "olcPixelGameEngine.h"

void SortTriangles(vector<Triangle>& vecToSort)
{
  sort(vecToSort.begin(), vecToSort.end(), [](Triangle& tri1, Triangle& tri2)
  {
    float zPoint1 = (tri1.points[0].z + tri1.points[1].z + tri1.points[2].z) / 3.0f;
    float zPoint2 = (tri2.points[0].z + tri2.points[1].z + tri2.points[2].z) / 3.0f;
    return zPoint1 > zPoint2;
  });
}

void DrawTexturedTriangle(const RenderingInstance& RI, const Triangle& input, const olc::Sprite* texture)
{
  Triangle tri = input;
  if(tri.points[1].y < tri.points[0].y)
  {
    swap(tri.points[0].y, tri.points[1].y);
    swap(tri.points[0].x, tri.points[1].x);
    swap(tri.texels[0].u, tri.texels[1].u);
    swap(tri.texels[0].v, tri.texels[1].v);
    swap(tri.texels[0].w, tri.texels[1].w);
  }
  
  if(tri.points[2].y < tri.points[0].y)
  {
    swap(tri.points[0].y, tri.points[2].y);
    swap(tri.points[0].x, tri.points[2].x);
    swap(tri.texels[0].u, tri.texels[2].u);
    swap(tri.texels[0].v, tri.texels[2].v);
    swap(tri.texels[0].w, tri.texels[2].w);
  }

  if(tri.points[2].y < tri.points[1].y)
  {
    swap(tri.points[1].y, tri.points[2].y);
    swap(tri.points[1].x, tri.points[2].x);
    swap(tri.texels[1].u, tri.texels[2].u);
    swap(tri.texels[1].v, tri.texels[2].v);
    swap(tri.texels[1].w, tri.texels[2].w);
  }
  
  int dx1 = tri.points[1].x - tri.points[0].x;
  int dy1 = tri.points[1].y - tri.points[0].y;
  float du1 = tri.texels[1].u - tri.texels[0].u;
  float dv1 = tri.texels[1].v - tri.texels[0].v;
  float dw1 = tri.texels[1].w - tri.texels[0].w;

  int dx2 = tri.points[2].x - tri.points[0].x;
  int dy2 = tri.points[2].y - tri.points[0].y;
  float du2 = tri.texels[2].u - tri.texels[0].u;
  float dv2 = tri.texels[2].v - tri.texels[0].v;
  float dw2 = tri.texels[2].w - tri.texels[0].w;

  float texU, texV, texW;

  float dAxStep = 0, dBxStep = 0,
        dU1Step = 0, dV1Step = 0,
        dU2Step = 0, dV2Step = 0,
        dW1Step = 0, dW2Step = 0;

  if(dy1)
  {
    dAxStep = dx1 / (float)abs(dy1);
    dU1Step = du1 / (float)abs(dy1);
    dV1Step = dv1 / (float)abs(dy1);
    dW1Step = dw1 / (float)abs(dy1);
  }

  if(dy2)
  {
    dBxStep = dx2 / (float)abs(dy2);
    dU2Step = du2 / (float)abs(dy2);
    dV2Step = dv2 / (float)abs(dy2);
    dW2Step = dw2 / (float)abs(dy2);
  }

  //Fill top half of triangle
  //cout << "DY1(top half): " << dy1 << '\n';
  if(dy1)
  {
    for(int i = tri.points[0].y; i <= tri.points[1].y; i++)
    {
      int ax = tri.points[0].x + (float)(i - tri.points[0].y) * dAxStep;
      int bx = tri.points[0].x + (float)(i - tri.points[0].y) * dBxStep;

      float texSu = tri.texels[0].u + (float)(i - tri.points[0].y) * dU1Step;
      float texSv = tri.texels[0].v + (float)(i - tri.points[0].y) * dV1Step;
      float texSw = tri.texels[0].w + (float)(i - tri.points[0].y) * dW1Step;

      float texEu = tri.texels[0].u + (float)(i - tri.points[0].y) * dU2Step;
      float texEv = tri.texels[0].v + (float)(i - tri.points[0].y) * dV2Step;
      float texEw = tri.texels[0].w + (float)(i - tri.points[0].y) * dW2Step;

      if(ax > bx)
      {
        swap(ax, bx);
        swap(texSu, texEu);
        swap(texSv, texEv);
        swap(texEw, texSw);
      }

      texU = texSu;
      texV = texSv;
      texW = texSw;
      float tStep = 1.0f / ((float)(bx - ax));
      float t = 0.0f;

      //Actually draw the triangle 
      for(int jFirst = ax; jFirst < bx; jFirst++)
      {
        texU = (1.0f - t) * texSu + t * texEu;
        texV = (1.0f - t) * texSv + t * texEv;
        texW = (1.0f - t) * texSw + t * texEw;
        //cout << "texU: " << texU << '\n';
        //cout << "texV: " << texV << '\n';
        olc::Pixel color = texture->Sample(texU / texW, texV / texW);
        //engine->FillRect({100,100},{300,300}, olc::GREEN);
        //cout << "(Top Half)J: " << j << ' ' << "I: " << i << '\n';
        //PrintColor(color);
        //cout << '\n';
        int indexedPoint = i * RI.engine->ScreenWidth() + jFirst;
        if(texW > RI.depthBuffer[indexedPoint])
        {  
          RI.engine->Draw({jFirst,i}, color);
          RI.depthBuffer[indexedPoint] = texW;
        }
        t += tStep;
      }
    }
  }

  dy1 = tri.points[2].y - tri.points[1].y;
  dx1 = tri.points[2].x - tri.points[1].x;
  dv1 = tri.texels[2].v - tri.texels[1].v;
  du1 = tri.texels[2].u - tri.texels[1].u;
  dw1 = tri.texels[2].w - tri.texels[1].w;

  if(dy1)
  {
    dAxStep = dx1 / (float)abs(dy1);
    dU1Step = du1 / (float)abs(dy1);
    dV1Step = dv1 / (float)abs(dy1);
    dW1Step = dw1 / (float)abs(dy1);
  }
    
  if(dy2)
  {
    dBxStep = dx2 / (float)abs(dy2);
  }
  
  //cout << "DY1 (bottom half): " << dy1 << '\n';
  if(dy1)
  {
    for(int i = tri.points[1].y; i <= tri.points[2].y; i++)
    {
      int ax = tri.points[1].x + (float)(i - tri.points[1].y) * dAxStep;
      int bx = tri.points[0].x + (float)(i - tri.points[0].y) * dBxStep;

      float texSu = tri.texels[1].u + (float)(i - tri.points[1].y) * dU1Step;
      float texSv = tri.texels[1].v + (float)(i - tri.points[1].y) * dV1Step;
      float texSw = tri.texels[1].w + (float)(i - tri.points[1].y) * dW1Step;
      
      float texEu = tri.texels[0].u + (float)(i - tri.points[0].y) * dU2Step;
      float texEv = tri.texels[0].v + (float)(i - tri.points[0].y) * dV2Step;
      float texEw = tri.texels[0].w + (float)(i - tri.points[0].y) * dW2Step;

      if(ax > bx)
      {
        swap(ax, bx);
        swap(texSu, texEu);
        swap(texSv, texEv);
        swap(texSw, texEw);
      }

      texU = texSu;
      texV = texSv;
      texW = texSw;
      float tStep = 1.0f / ((float)(bx - ax));
      float t = 0.0f;

      //Actually draw the triangle(bottom half)
      for(int jSecond = ax; jSecond < bx; jSecond++)
      {
        texU = (1.0f - t) * texSu + t * texEu;
        texV = (1.0f - t) * texSv + t * texEv;
        texW = (1.0f - t) * texSw + t * texEw;
        olc::Pixel color = texture->Sample(texU / texW, texV / texW);
        //cout << "(Bottom Half)J: " << j << ' ' << "I: " << i << '\n';
        //PrintColor(color);
        //cout << '\n';
        int indexedPoint = i * RI.engine->ScreenWidth() + jSecond;
        if(texW > RI.depthBuffer[indexedPoint])
        {
          RI.engine->Draw({jSecond,i}, color);
          RI.depthBuffer[indexedPoint] = texW;
        }
        t += tStep;
      }
    }
  }
}

Matrix4x4 DoInputLoop(olc::PixelGameEngine* engine, const float& fElapsedTime, float& fYaw, float& fPitch, Vector3D& CAMERA, Vector3D& LookDirection)
{
  //Input loop
  if(engine->GetKey(olc::Key::DOWN).bHeld)
    CAMERA.y -= CAMERA_VERTICAL_SPEED * fElapsedTime;
    
  if(engine->GetKey(olc::Key::UP).bHeld)
    CAMERA.y += CAMERA_VERTICAL_SPEED * fElapsedTime;

  if(engine->GetKey(olc::Key::LEFT).bHeld)
    fYaw += CAMERA_ROTATION_SPEED * fElapsedTime;

  if(engine->GetKey(olc::Key::RIGHT).bHeld)
    fYaw -= CAMERA_ROTATION_SPEED * fElapsedTime;

  if(engine->GetKey(olc::Key::P).bHeld && fPitch <= 45.0f)
    fPitch += CAMERA_ROTATION_SPEED * 0.5 * fElapsedTime;

  if(engine->GetKey(olc::Key::L).bHeld && fPitch >= -45.0f)
    fPitch -= CAMERA_ROTATION_SPEED * 0.5 * fElapsedTime;

  //Getting the camera to traverse in the relative forward direction is more involved
  //We will create a new vector in the look direciton of the camera and scale its
  //This is essentially a velocity vector
  Vector3D tempForward = LookDirection;
  MultiplyVectorScalar(tempForward, CAMERA_HORIZONTAL_SPEED * fElapsedTime);
  if(engine->GetKey(olc::Key::W).bHeld && engine->GetKey(olc::Key::A).bHeld)
    CAMERA = AddVector(CAMERA, tempForward);

  else if(engine->GetKey(olc::Key::W).bHeld && engine->GetKey(olc::Key::D).bHeld)
    CAMERA = AddVector(CAMERA, tempForward);

  else if(engine->GetKey(olc::Key::W).bHeld)
    CAMERA = AddVector(CAMERA, tempForward); 

  if(engine->GetKey(olc::Key::S).bHeld && engine->GetKey(olc::Key::A).bHeld)
    CAMERA = SubtractVector(CAMERA, tempForward);

  else if(engine->GetKey(olc::Key::S).bHeld && engine->GetKey(olc::Key::D).bHeld)
    CAMERA = SubtractVector(CAMERA, tempForward);

  else if(engine->GetKey(olc::Key::S).bHeld)
    CAMERA = SubtractVector(CAMERA, tempForward);

  Vector3D rightVector = GetCrossProduct(LookDirection, UP_DIRECTION);
  MultiplyVectorScalar(rightVector, CAMERA_HORIZONTAL_SPEED * fElapsedTime);
  if(engine->GetKey(olc::Key::A).bHeld)
    CAMERA = AddVector(CAMERA,rightVector);

  if(engine->GetKey(olc::Key::D).bHeld)
    CAMERA = SubtractVector(CAMERA, rightVector);

  Vector3D TARGET = {0.0f,0.0f, 1.0f};
  Vector3D tempLookDirection = LookDirection;
  Matrix4x4 cameraYRotationMatrix = GetRotationMatrix(ROT_TYPES::ROT_Y, fYaw * (mathPI / 180));
  //LookDirection is now updated in the Y direction
  MultiplyMatrixVector(TARGET, cameraYRotationMatrix, tempLookDirection);

  //Get the local X-axis vector
  Vector3D localXAxis = GetCrossProduct(tempLookDirection, UP_DIRECTION);
  Matrix4x4 localXAxisRotation = GetArbitraryRotationMatrix(localXAxis, fPitch * (mathPI / 180));

  //Update tempLookDirection and put its value in the final LookDirection
  MultiplyMatrixVector(tempLookDirection,localXAxisRotation, LookDirection);

  TARGET = AddVector(CAMERA, LookDirection);
  Vector3D tempCam;
  Matrix4x4 cameraMatrix = GetPointAtMatrix(CAMERA, TARGET, UP_DIRECTION);
  Matrix4x4 viewMatrix  = InvertPointAtMatrix(cameraMatrix); 

  return viewMatrix;
}

void ClearScreenPerformance(olc::PixelGameEngine* engine, const vector<Triangle>& trianglesToRaster, const vector<Vector3D>& normalsToRaster)
{
  //These are the previous points on the screen that we should clear
  olc::vf2d pointTriPrevious1;
  olc::vf2d pointTriPrevious2;
  olc::vf2d pointTriPrevious3;
  olc::vf2d pointNormPrevious1;
  olc::vf2d pointNormPrevious2;

  //I think iterating over the 2 vectors is more cache efficient
  if(SETTINGS_MAP[DRAW_FACES] == true || SETTINGS_MAP[DRAW_LINES] == true)
  {
    for(int i = 0; i < trianglesToRaster.size(); i++)
    {
      pointTriPrevious1.x = trianglesToRaster[i].points[0].x; pointTriPrevious1.y = trianglesToRaster[i].points[0].y;
      pointTriPrevious2.x = trianglesToRaster[i].points[1].x; pointTriPrevious2.y = trianglesToRaster[i].points[1].y;
      pointTriPrevious3.x = trianglesToRaster[i].points[2].x; pointTriPrevious3.y = trianglesToRaster[i].points[2].y;
      engine->FillTriangle(pointTriPrevious1, pointTriPrevious2, pointTriPrevious3, olc::BLACK);
    }
  }

  if(SETTINGS_MAP[DRAW_NORMALS] == true)
  {
    for(int i = 0; i < normalsToRaster.size(); i++)
    {
      pointNormPrevious1.x = normalsToRaster[i].x; pointNormPrevious2.y = normalsToRaster[i].y;
      pointNormPrevious2.x = trianglesToRaster[i].points[0].x;
      pointNormPrevious2.y = trianglesToRaster[i].points[0].y;
      engine->DrawLine(pointNormPrevious1, pointNormPrevious2, olc::BLACK);
    }
  }
}

float GetNoneMaterialLuminances(const Vector3D& normal, const deque<Light>& lightsDeque)
{
  float currentLuminance = 0.0f;
  for(const auto& light : lightsDeque)
{
    switch(light.GetLightType())
    {
      case LIGHT_TYPES::SUN:
        {
          currentLuminance += GetDotProduct(normal, light.GetDirection()) * light.intensity;
          break;
        }
      case LIGHT_TYPES::POINT:
        {
          cerr << "ERROR: Point light not implemented yet\n";
          break;
        }
      default:
        {
          cerr << "ERROR: Invalid lamp type specified during runtime. Please report to developer\n";
          break;
        }
   }
  }
  //cout << "currentLuminance: " << currentLuminance << '\n';
  return max(MINIMUM_NONE_LUMINANCE, currentLuminance);
}

olc::Pixel GetDiffuseMaterialColor(const Vector3D& normal, const olc::Pixel& diffuseColor, const deque<Light>& lightsDeque)
{
  olc::Pixel toReturn;
  float rVal = 0.0f;
  float gVal = 0.0f;
  float bVal = 0.0f;
  for(const auto& light : lightsDeque)
  {
    float colorLightIntensity = max(GetDotProduct(normal, light.GetDirection()), MINIMUM_DIFFUSE_LUMINANCE);
    switch(light.GetLightType())
    {
      case LIGHT_TYPES::SUN:
        {
          rVal += diffuseColor.r * light.color.r * light.intensity * colorLightIntensity * SUN_DIVISION_CONSTANT;
          gVal += diffuseColor.g * light.color.g * light.intensity * colorLightIntensity * SUN_DIVISION_CONSTANT;
          bVal += diffuseColor.b * light.color.b * light.intensity * colorLightIntensity * SUN_DIVISION_CONSTANT;
          break;
        }
      case LIGHT_TYPES::POINT:
        {
          cerr << "ERROR: Point light not implemented yet\n";
          break;
        }
      default:
        {
          cerr << "ERROR: Invalid lamp type specified during runtime. Please report to developer\n";
          break;
        }
    }
    toReturn.r = clamp(rVal, MINIMUM_DIFFUSE_COLOR, 255.0f);
    toReturn.g = clamp(gVal, MINIMUM_DIFFUSE_COLOR, 255.0f);
    toReturn.b = clamp(bVal, MINIMUM_DIFFUSE_COLOR, 255.0f);
  }
  return toReturn;
}

void DrawTriangleToScreen(const RenderingInstance& RI, const Triangle& triangleInput, const short& materialType, const olc::Decal* texture)
{
  olc::vf2d point1, point2, point3;

  //Rasterizing triangle
  if(SETTINGS_MAP[DRAW_FACES] == true)
  {
    //If visualize clipping is enabled, then we should draw the triangles color, not their actual
    //material
    if(SETTINGS_MAP[VISUALIZE_CLIPPING] == true)
    {
      PopulateOLCPoints(triangleInput, point1, point2, point3);
      RI.engine->FillTriangle(point1, point2, point3, triangleInput.color);
      return;
    }
    switch(materialType)
    {
      case MATERIAL_TYPES::TEXTURE:
        {
          //Just incase
          if(texture != nullptr)
            DrawTexturedTriangle(RI, triangleInput, texture->sprite);

          else
          {
            DrawTexturedTriangle(RI, triangleInput, MISSING_TEXTURE_SPRITE);
          }
          break;
        }
      case MATERIAL_TYPES::COMPOSITE:
        {
          break;
        }
      default:
        {
          PopulateOLCPoints(triangleInput, point1, point2, point3);
          RI.engine->FillTriangle(point1, point2 , point3, triangleInput.color);
          break;
        }
    }
  }
  
  //Drawing Wireframe
  if(SETTINGS_MAP[DRAW_LINES] == true)
  {
    PopulateOLCPoints(triangleInput, point1, point2, point3);
    RI.engine->DrawTriangle(point1, point2, point3, WIREFRAME_COLOR);
  }
}

void PopulateOLCPoints(const Triangle& inputTriangle, olc::vf2d& point1, olc::vf2d& point2, olc::vf2d& point3)
{
  point1.x = inputTriangle.points[0].x; point1.y = inputTriangle.points[0].y;
  point2.x = inputTriangle.points[1].x; point2.y = inputTriangle.points[1].y;
  point3.x = inputTriangle.points[2].x; point3.y = inputTriangle.points[2].y;
}

