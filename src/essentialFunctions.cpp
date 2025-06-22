#include <vector>
#include <algorithm>
#include "inputManager.h"
#include "matrixMathEssentials.h"
#include "essentialFunctions.h"
#include "geometricPrimitives.h"
#include "miscFunctions.h"
#include "miscPrimitives.h"
#include "vectorMathEssentials.h"
#include "globalVariables.h"
#include "olcPixelGameEngine.h"
#include "normalMathEssentials.h"
#include "inputManager.h"
#include "drawingRoutines.h"

void SortTriangles(vector<Triangle>& vecToSort)
{
  sort(vecToSort.begin(), vecToSort.end(), [](Triangle& tri1, Triangle& tri2)
  {
    float zPoint1 = (tri1.points[0].z + tri1.points[1].z + tri1.points[2].z) / 3.0f;
    float zPoint2 = (tri2.points[0].z + tri2.points[1].z + tri2.points[2].z) / 3.0f;
    return zPoint1 > zPoint2;
  });
}

Matrix4x4 DoInputLoop(olc::PixelGameEngine* engine, Player* player)
{
  //Variable aliases for readability
  Vector3D& position = player->camera.cameraPosition;
  Vector3D& CAMERA = player->camera.cameraPosition;
  const float& fElapsedTime = engine->GetElapsedTime();
  float& fYaw = player->camera.fYaw;
  float& fPitch = player->camera.fPitch;
  const Vector3D& LookDirection = player->camera.GetFacingVector();
  const float& CAMERA_VERTICAL_SPEED = player->camera.GetVerticalSpeed();
  const float& CAMERA_HORIZONTAL_SPEED = player->camera.GetHorizontalSpeed();
  const float& CAMERA_ROTATION_SPEED = player->camera.GetRotationSpeed();

  //Print currently pressed inputs
  InputManager::PrintPressedKeys(engine);

  //Move down
  if(InputManager::KeyHeld(engine, {MOVE_DOWN}))
    CAMERA.y -= CAMERA_VERTICAL_SPEED * fElapsedTime;
    
  //Move up
  else if(InputManager::KeyHeld(engine, {MOVE_UP}))
    CAMERA.y += CAMERA_VERTICAL_SPEED * fElapsedTime;

  //Add Yaw (Look left. Aka, rotate left along y axis)
  if(InputManager::KeyHeld(engine, {LOOK_LEFT}))
    fYaw -= CAMERA_ROTATION_SPEED * fElapsedTime;
  
  //Subtract Yaw (Look right. Aka, Rotate right along y axis)
  else if(InputManager::KeyHeld(engine, {LOOK_RIGHT}))
    fYaw += CAMERA_ROTATION_SPEED * fElapsedTime;

  //Add pitch (Look up. Aka, rotate up along local x axis)
  if(InputManager::KeyHeld(engine, {ROTATE_UP}) && fPitch <= 45.0f)
    fPitch += CAMERA_ROTATION_SPEED * 0.4f * fElapsedTime;

  //Remove pitch (look down. Aka, rotate down along local x axis)
  else if(InputManager::KeyHeld(engine,{ROTATE_DOWN}) && fPitch >= -45.0f)
    fPitch -= CAMERA_ROTATION_SPEED * 0.4f * fElapsedTime;

  //This is where we will deal with forward and backward movement
  //Getting the camera to traverse in the relative forward direction is more involved
  //We will create a new vector in the look direciton of the camera and scale its
  //This is essentially a velocity vector

  Vector3D tempForward = LookDirection;
  MultiplyVectorScalar(tempForward, CAMERA_HORIZONTAL_SPEED * fElapsedTime);
  
   // Initialize movement vector
    Vector3D desiredMovement = {0.0f, 0.0f, 0.0f};

  // Calculate forward and backward movement
  if(InputManager::KeyHeld(engine, {MOVE_FORWARD}))
    AddVectorIP(desiredMovement, LookDirection);
  
  else if(InputManager::KeyHeld(engine, {MOVE_BACKWARD}))
    SubtractVectorIP(desiredMovement, LookDirection);

  // Calculate side movements
  Vector3D rightVector = GetCrossProduct(UP_DIRECTION, LookDirection);
  if(InputManager::KeyHeld(engine, {MOVE_LEFT}))
    AddVectorIP(desiredMovement, rightVector);
  
  else if(InputManager::KeyHeld(engine, {MOVE_RIGHT}))
    SubtractVectorIP(desiredMovement, rightVector);

  // Normalize the desired movement vector if it's not zero to prevent div by 0 exception
  if(IsZeroVector(desiredMovement) == false)
  {
    NormalizeVector(desiredMovement);
    MultiplyVectorScalar(desiredMovement, CAMERA_HORIZONTAL_SPEED * fElapsedTime);
    AddVectorIP(CAMERA, desiredMovement);
  }

  Vector3D TARGET = {0.0f,0.0f, 1.0f};
  Vector3D newLookDirection;
  Matrix4x4 cameraYRotationMatrix = GetRotationMatrix(ROT_TYPES::ROT_Y, fYaw * (mathPI / 180));

  //LookDirection is now updated in the Y direction
  MultiplyMatrixVector(TARGET, cameraYRotationMatrix, newLookDirection);

  TARGET = AddVector(CAMERA, newLookDirection);
  //Set the facing vector to the new one
  player->camera.SetFacingVector(newLookDirection);
  Matrix4x4 cameraMatrix = GetPointAtMatrix(CAMERA, TARGET, UP_DIRECTION);
  Matrix4x4 viewMatrix  = InvertPointAtMatrix(cameraMatrix); 

  return viewMatrix;
}

void ClearScreenPerformance(olc::PixelGameEngine* engine, const vector<Triangle>& trianglesToRaster)
{
  /*
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
  */
}

float GetNoneMaterialLuminances(const Vector3D& normal, const deque<Light>& lightsDeque)
{
  float currentLuminance = 0.0f;
  for(const auto& light : lightsDeque)
  {
    switch(light.GetLightType())
    {
      case LIGHT_TYPES::LAMP_SUN:
        {
          currentLuminance += GetDotProduct(normal, light.GetDirection()) * light.intensity;
          break;
        }
      case LIGHT_TYPES::LAMP_POINT:
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
      case LIGHT_TYPES::LAMP_SUN:
        {
          rVal += diffuseColor.r * light.color.r * light.intensity * colorLightIntensity * SUN_DIVISION_CONSTANT;
          gVal += diffuseColor.g * light.color.g * light.intensity * colorLightIntensity * SUN_DIVISION_CONSTANT;
          bVal += diffuseColor.b * light.color.b * light.intensity * colorLightIntensity * SUN_DIVISION_CONSTANT;
          break;
        }
      case LIGHT_TYPES::LAMP_POINT:
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
    switch(materialType)
    {
      case MATERIAL_TYPES::TEXTURE:
        {
          //Just incase
          if(texture != nullptr)
          {
            DrawTexturedTriangle(RI, triangleInput, texture->sprite);
          }

          else
          {
            DrawTexturedTriangle(RI, triangleInput, MISSING_TEXTURE_SPRITE);
          }
          break;
        }
      case MATERIAL_TYPES::COMPOSITE:
        {
          cerr << "ERROR: Composite material type note yet supported!\n";
          break;
        }
      case MATERIAL_TYPES::NONE:
        {

        }
      case MATERIAL_TYPES::DIFFUSE:
        {
          PopulateOLCPoints(triangleInput, point1, point2, point3);
          //RI.engine->FillTriangle(point1, point2 , point3, triangleInput.color);
          FillTriangleWithDepthBuffer(triangleInput, RI, triangleInput.color);
          break;
        }
      default:
        {
          cerr << "ERROR: Material type not specified. Please file a bug report immiedtly!\n";
        }
    }
  }
  
  //Drawing Wireframe
  if(SETTINGS_MAP[DRAW_LINES] == true)
  {
    //PopulateOLCPoints(triangleInput, point1, point2, point3);
    //RI.engine->DrawTriangle(point1, point2, point3, WIREFRAME_COLOR);
    DrawTriangleWithDepthBuffer(triangleInput, RI, WIREFRAME_COLOR);
  }
}

void PopulateOLCPoints(const Triangle& inputTriangle, olc::vf2d& point1, olc::vf2d& point2, olc::vf2d& point3)
{
  point1.x = inputTriangle.points[0].x; point1.y = inputTriangle.points[0].y;
  point2.x = inputTriangle.points[1].x; point2.y = inputTriangle.points[1].y;
  point3.x = inputTriangle.points[2].x; point3.y = inputTriangle.points[2].y;
}

void DoAuxilliaryInputLoop(olc::PixelGameEngine* engine)
{
  if(InputManager::KeyHeld(engine, {SPECIAL_ONE}) && InputManager::KeyHeld(engine,{SPECIAL_TWO}) && engine->GetKey(olc::Key::S).bHeld)
  {
    TakeScreenshot(engine);
  }
}

Vector3D GetProjectedNormal(olc::PixelGameEngine* engine, const Matrix4x4& projectionMatrix, const Triangle& rawTriangleInput, const Vector3D& normalInput)
{
  Vector3D scaledNormal = ScaleNormal(rawTriangleInput.points[1], normalInput, 1.0f);
  Vector3D projectedNormal;
  MultiplyMatrixVector(scaledNormal, projectionMatrix, projectedNormal);
    
  //Converting to DNC Coordinates
  DivideVectorScalar(projectedNormal, projectedNormal.w);
    
  //Fixing Inverted Y axis
  projectedNormal.y *= -1.0f;

  //Normalizing to Screen Size
  projectedNormal.x += 1.0f; projectedNormal.y += 1.0f;
  projectedNormal.x *= 0.5f * (float)engine->ScreenWidth(); 
  projectedNormal.y *= 0.5f * (float)engine->ScreenHeight();
  return projectedNormal;
}
