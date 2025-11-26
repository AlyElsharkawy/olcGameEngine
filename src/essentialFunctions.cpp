#include <numbers>
#include <optional>
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
  //InputManager::PrintPressedKeys(engine);

  //Move down
  if(InputManager::KeyHeld(engine, {MOVE_DOWN}))
    CAMERA.y -= CAMERA_VERTICAL_SPEED * fElapsedTime;
    
  //Move up
  else if(InputManager::KeyHeld(engine, {MOVE_UP}))
    CAMERA.y += CAMERA_VERTICAL_SPEED * fElapsedTime;

  //Add Yaw (Look left. Aka, rotate left along y axis)
  if(InputManager::KeyHeld(engine, {LOOK_LEFT}))
    fYaw += CAMERA_ROTATION_SPEED * fElapsedTime;
  
  //Subtract Yaw (Look right. Aka, Rotate right along y axis)
  else if(InputManager::KeyHeld(engine, {LOOK_RIGHT}))
    fYaw -= CAMERA_ROTATION_SPEED * fElapsedTime;

  //Add pitch (Look up. Aka, rotate up along local x axis)
  if(InputManager::KeyHeld(engine, {ROTATE_UP}) && fPitch <= 45.0f)
    fPitch += CAMERA_ROTATION_SPEED * 0.4f * fElapsedTime;

  //Remove pitch (look down. Aka, rotate down along local x axis)
  else if(InputManager::KeyHeld(engine,{ROTATE_DOWN}) && fPitch >= -45.0f)
    fPitch -= CAMERA_ROTATION_SPEED * 0.4f * fElapsedTime;

  //This is where we will deal with forward and backward movement
  //Getting the camera to traverse in the relative forward direction is more involved
  //We will create a new vector in the look direction of the camera and scale its
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
    SubtractVectorIP(desiredMovement, rightVector);
  
  else if(InputManager::KeyHeld(engine, {MOVE_RIGHT}))
    AddVectorIP(desiredMovement, rightVector);

  // Normalize the desired movement vector if it's not zero to prevent div by 0 exception
  if(IsZeroVector(desiredMovement) == false)
  {
    NormalizeVector(desiredMovement);
    MultiplyVectorScalar(desiredMovement, CAMERA_HORIZONTAL_SPEED * fElapsedTime);
    AddVectorIP(CAMERA, desiredMovement);
  }

  Vector3D TARGET = {0.0f,0.0f, 1.0f};
  Vector3D newLookDirection;
  Matrix4x4 cameraYRotationMatrix = GetRotationMatrix(ROT_TYPES::ROT_Y, fYaw * (numbers::pi / 180));

  //LookDirection is now updated in the Y direction
  MultiplyMatrixVector(TARGET, cameraYRotationMatrix, newLookDirection);

  TARGET = AddVector(CAMERA, newLookDirection);
  //Set the facing vector to the new one
  player->camera.SetFacingVector(newLookDirection);
  Matrix4x4 cameraMatrix = GetPointAtMatrix(CAMERA, TARGET, UP_DIRECTION);
  Matrix4x4 viewMatrix  = InvertPointAtMatrix(cameraMatrix); 

  return viewMatrix;
}

float GetNoneMaterialLuminances(const Vector3D& normal, const deque<Light*>& lightsDeque)
{
  float currentLuminance = 0.0f;
  for(const auto& light : lightsDeque)
  {
    switch(light->GetLightType())
    {
      case LIGHT_TYPES::LAMP_SUN:
        {
          currentLuminance += GetDotProduct(normal, light->GetDirection()) * light->intensity;
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

olc::Pixel GetDiffuseMaterialColor(const Vector3D& normal, const olc::Pixel& diffuseColor, const deque<Light*>& lightsDeque)
{
  olc::Pixel toReturn;
  float rVal = 0.0f;
  float gVal = 0.0f;
  float bVal = 0.0f;
  float colorLightIntensity = 0.0f;
  float normalizedRDiffuse, normalizedGDiffuse, normalizedBDiffuse;
  float normalizedRLight, normalizedGLight, normalizedBLight;
  for(const auto& light : lightsDeque)
  {
    switch(light->GetLightType())
    {
      case LIGHT_TYPES::LAMP_SUN:
        {
          Vector3D lightDirection = light->GetDirection();
          MultiplyVectorScalar(lightDirection, -1.0f);
          colorLightIntensity = max(GetDotProduct(normal, lightDirection), 0.0f);
          if(colorLightIntensity == 0.0f)
            continue;

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

    normalizedRDiffuse = diffuseColor.r / 255.0f;
    normalizedGDiffuse = diffuseColor.g / 255.0f;
    normalizedBDiffuse = diffuseColor.b / 255.0f;

    normalizedRLight = light->GetNormalizedColorCodes()[0];
    normalizedGLight = light->GetNormalizedColorCodes()[1];
    normalizedBLight = light->GetNormalizedColorCodes()[2];

    rVal += normalizedRDiffuse * normalizedRLight * colorLightIntensity * light->intensity;
    gVal += normalizedGDiffuse * normalizedGLight * colorLightIntensity * light->intensity;
    bVal += normalizedBDiffuse * normalizedBLight * colorLightIntensity * light->intensity;
  }
  toReturn.r = clamp(rVal * 255.0f, MINIMUM_DIFFUSE_COLOR, 255.0f);
  toReturn.g = clamp(gVal * 255.0f, MINIMUM_DIFFUSE_COLOR, 255.0f);
  toReturn.b = clamp(bVal * 255.0f, MINIMUM_DIFFUSE_COLOR, 255.0f);
  return toReturn;
}

NormalizedPixel GetPartiallyIlluminatedColorCode(const Vector3D& normal, const deque<Light*>& lightsDeque)
{
  float rVal = 0.0f;
  float gVal = 0.0f;
  float bVal = 0.0f;
  float colorLightIntensity = 0.0f;
  float normalizedRLight, normalizedGLight, normalizedBLight;
  for(const auto& light : lightsDeque)
  {
    switch(light->GetLightType())
    {
      case LIGHT_TYPES::LAMP_SUN:
        {
          Vector3D lightDirection = light->GetDirection();
          MultiplyVectorScalar(lightDirection, -1.0f);
          colorLightIntensity = max(GetDotProduct(normal, lightDirection), 0.0f);
          if(colorLightIntensity == 0.0f)
            continue;

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
    normalizedRLight = light->GetNormalizedColorCodes()[0];
    normalizedGLight = light->GetNormalizedColorCodes()[1];
    normalizedBLight = light->GetNormalizedColorCodes()[2];

    rVal += normalizedRLight * colorLightIntensity * light->intensity;
    gVal += normalizedGLight * colorLightIntensity * light->intensity;
    bVal += normalizedBLight * colorLightIntensity * light->intensity;
  }
  return NormalizedPixel(rVal, gVal, bVal);
}

void DrawTriangleToScreen(const RenderingInstance& RI, const Triangle& triangleInput,
                          const Vector3D& illuminationNormal, const deque<Light*>& lightsDeque, 
                          const short& materialType, const olc::Decal* texture)
{
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
            NormalizedPixel pixelIllumination = GetPartiallyIlluminatedColorCode(illuminationNormal, lightsDeque);
            DrawTexturedTriangle(RI, triangleInput, pixelIllumination, texture->sprite);
          }

          else
          {
            NormalizedPixel pixelIllumination = GetPartiallyIlluminatedColorCode(illuminationNormal, lightsDeque);
            DrawTexturedTriangle(RI, triangleInput, pixelIllumination, MISSING_TEXTURE_SPRITE);
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
          olc::vf2d point1, point2, point3;
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

void DrawNormal(const RenderingInstance& RI, const Vector3D& normal, const Triangle& triangleSource)
{
  const float& x1 = triangleSource.points[1].x;
  const float& y1 = triangleSource.points[1].y;
  const float& x2 = normal.x;
  const int& y2 = normal.y;
  float w1 = triangleSource.points[1].w;
  float w2 = normal.w;
  DrawLineWithDepthBufferInline(x1, y1, 1.0f / w1, x2, y2, 1.0f / w2, RI, NORMAL_COLOR);
}

void DoAuxiliaryInputLoop(olc::PixelGameEngine* engine, MeshList& allObjects, deque<Light*>& allLights)
{
  if(InputManager::KeyHeld(engine, {SPECIAL_ONE}) && InputManager::KeyHeld(engine,{SPECIAL_TWO}) && engine->GetKey(olc::Key::S).bHeld)
  {
    TakeScreenshot(engine);
  }
  SetInitialObjects(engine, allObjects, allLights);
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

void ClearAllObjectsandLights(MeshList& allObjects, deque<Light*>& allLights)
{
  for(const auto& elm : allObjects.GetMeshList())
  {
    if(elm != nullptr)
      delete elm;
  }

  for(const auto& elm : allLights)
  {
    if(elm != nullptr)
      delete elm;
  }
  allObjects.GetMeshList().clear();
  allLights.clear();
}

void CreateStandardSunLamp(deque<Light*>& allLights)
{
  Light* mainLamp = new Light(LIGHT_TYPES::LAMP_SUN, {0.0f, -1.0f, 1.0f}, {255, 255, 255}, 1.0f);
  allLights.push_back(mainLamp);
}

void SetInitialObjects(olc::PixelGameEngine* engine, MeshList& allObjects, deque<Light*>& allLights, std::optional<BASIC_CONTROLS_ENUM> sceneNumber)
{
  int chosenScene = -1;
  if(sceneNumber.has_value()) 
  {
    chosenScene = *sceneNumber;
  }

  //Standard testing environment
  if(InputManager::KeyHeld(engine, {NUM_1}) || chosenScene == BASIC_CONTROLS_ENUM::NUM_1)
  {
    if(currentObjectSet == 1)
      return;

    ClearAllObjectsandLights(allObjects, allLights);
    CreateStandardSunLamp(allLights);

    //Initialize hard coded meshes
    Mesh* testMesh = new Mesh();
    testMesh->LoadFromOBJFile(GetPathFromResources({"objectFiles", "Primitives", "GoodCube.obj"}), true);
    testMesh->SetTranslationOffsets(0.0f,0.0f, 0.0f);
    testMesh->SetRotationSpeeds(1.0f, 0.0f, 0.0f);
    testMesh->SetTextureImage(GetPathFromResources({"textures", "stoneBrickWall.png"}));

    Mesh* testMesh2 = testMesh->Duplicate();
    testMesh2->SetTranslationOffsets(0, 0, 3);
    testMesh2->SetDiffuseColor(210, 4, 45, 255);
    
    Mesh* testMesh3 = testMesh2->Duplicate();
    testMesh3->SetDiffuseColor(0, 0, 139, 255);
    testMesh3->SetTranslationOffsets(0, 3, 0);
    testMesh3->doAutomaticRotation = true;

    Mesh* bunny = new Mesh();
    bunny->LoadFromOBJFile(GetPathFromResources({"objectFiles", "Primitives", "bunny.obj"}));
    bunny->doAutomaticRotation = true;
    bunny->doAutomaticRotations[1] = true;
    bunny->SetTranslationOffsets(0, 0, 9);
    bunny->SetScalingOffsets(60, 60, 60);
    uint8_t rVal, bVal, gVal;
    HexToRGB("d5e1f0", rVal, gVal, bVal);
    bunny->SetDiffuseColor(rVal, gVal, bVal, 255);
    
    allObjects.AppendMesh(testMesh);
    allObjects.AppendMesh(testMesh2);
    allObjects.AppendMesh(testMesh3);
    allObjects.AppendMesh(bunny);
    allObjects.UpdateTotalCounts();
    currentObjectSet = 1;
  }

  //Texturing testing environment
  else if(InputManager::KeyHeld(engine, {NUM_2}) || chosenScene == BASIC_CONTROLS_ENUM::NUM_2)
  {
    if(currentObjectSet == 2)
      return;

    ClearAllObjectsandLights(allObjects, allLights);
    CreateStandardSunLamp(allLights);

    Mesh* dirtCube = new Mesh();
    dirtCube->LoadFromOBJFile(GetPathFromResources({"objectFiles", "Primitives", "GoodCube.obj"}), true);
    dirtCube->SetTranslationOffsets(-2.0f, 0, 0.0f);
    dirtCube->SetTextureImage(GetPathFromResources({"textures", "ground.png"}));
    dirtCube->rotationSpeeds[1] = 5.0f;
    dirtCube->doAutomaticRotation = true;
    dirtCube->doAutomaticRotations[1] = true;

    Mesh* stoneCube = new Mesh();
    stoneCube->LoadFromOBJFile(GetPathFromResources({"objectFiles", "Primitives", "GoodCube.obj"}), true);
    stoneCube->SetTranslationOffsets(2.0f, 0.0f, 0.0f);
    stoneCube->SetTextureImage(GetPathFromResources({"textures", "stoneBrickWall.png"}));
    stoneCube->doAutomaticRotation = true;
    stoneCube->rotationSpeeds[1] = 5.0f;
    stoneCube->doAutomaticRotations[1] = true;

    Mesh* brickCube = new Mesh();
    brickCube->LoadFromOBJFile(GetPathFromResources({"objectFiles", "Primitives", "GoodCube.obj"}), true);
    brickCube->SetTranslationOffsets(0.0f, 0.0f, 2.0f);
    brickCube->SetTextureImage(GetPathFromResources({"textures", "brickWall.png"}));
    brickCube->doAutomaticRotation = true;
    brickCube->doAutomaticRotations[1] = true;
    brickCube->rotationSpeeds[1] = 3.0f;

    Mesh* badCube = new Mesh();
    badCube = brickCube->Duplicate();
    badCube->SetTranslationOffsets(0.0, 2.5f, 3.0f);
    badCube->SetTextureImage(GetPathFromResources({"textures", "missingTexture.png"}));
    badCube->rotationSpeeds[1] = 3.0f;

    allObjects.AppendMesh(dirtCube);
    allObjects.AppendMesh(stoneCube);
    allObjects.AppendMesh(brickCube);
    allObjects.AppendMesh(badCube);
    allObjects.UpdateTotalCounts();
    currentObjectSet = 2;
  }

  //High poly test environment
  else if(InputManager::KeyHeld(engine, {NUM_3}) || chosenScene == BASIC_CONTROLS_ENUM::NUM_3)
  {
    if(currentObjectSet == 3)
      return;

    ClearAllObjectsandLights(allObjects, allLights);
    CreateStandardSunLamp(allLights);
    
    Mesh* suzanne1 = new Mesh();
    suzanne1->LoadFromOBJFile(GetPathFromResources({"objectFiles", "Primitives", "Monkey-High-Resolution.obj"}), false);
    suzanne1->SetScalingOffsets(1.5f, 1.5f, 1.5f);
    suzanne1->SetDiffuseColor(50, 205, 50, 255);
    suzanne1->SetTranslationOffsets(-3.0f, 0.0f, 3.0f);
    suzanne1->doAutomaticRotations[1] = true;
    suzanne1->doAutomaticRotation = true;
    suzanne1->rotationSpeeds[1] = 2.0f;

    Mesh* suzanne2 = new Mesh();
    suzanne2 = suzanne1->Duplicate();
    suzanne2->SetTranslationOffsets(3.0f, 0.0f, 3.0f);

    Mesh* sphere1 = new Mesh();
    sphere1->LoadFromOBJFile(GetPathFromResources({"objectFiles", "Primitives", "UV-Sphere-High-Resolution.obj"}), false);
    sphere1->SetTranslationOffsets(-2.0f, 0.0f, 0.0f);
    sphere1->SetDiffuseColor(210, 4, 45, 255);

    Mesh* sphere2 = new Mesh();
    sphere2 = sphere1->Duplicate();
    sphere2->SetTranslationOffsets(2.0f, 0.0f, 0.0f);

    allObjects.AppendMesh(suzanne1);
    allObjects.AppendMesh(suzanne2);
    allObjects.AppendMesh(sphere1);
    allObjects.AppendMesh(sphere2);
    currentObjectSet = 3;
  }

  //Large texturing test environment
  else if(InputManager::KeyHeld(engine, {NUM_4}) || chosenScene == BASIC_CONTROLS_ENUM::NUM_4)
  {
    if(currentObjectSet == 4)
      return;

    ClearAllObjectsandLights(allObjects, allLights);
    CreateStandardSunLamp(allLights);

    Mesh* cottageTest = new Mesh();
    cottageTest->LoadFromOBJFile(GetPathFromResources({"objectFiles", "Objects", "cottage_tri.obj"}), true);
    cottageTest->SetTextureImage(GetPathFromResources({"textures", "cottage_diffuse.png"}));
    cottageTest->SetScalingOffsets(0.5f, 0.5f, 0.5f);
    cottageTest->SetTranslationOffsets(0.0f, 0.0f, 10.0f);
    cottageTest->rotationDegrees[1] = numbers::pi;
  
    allObjects.AppendMesh(cottageTest);
    allObjects.UpdateTotalCounts();
    currentObjectSet = 4;
  }

  else if(InputManager::KeyHeld(engine, {NUM_0}) || chosenScene == BASIC_CONTROLS_ENUM::NUM_0)
  {
    if(currentObjectSet == 0)
        return;

    ClearAllObjectsandLights(allObjects, allLights);
    currentObjectSet = 0;
  }
}
