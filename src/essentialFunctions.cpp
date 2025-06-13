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
  float y1 = input.points[0].y;
  float y2 = input.points[1].y;
  float y3 = input.points[2].y;

  float z1 = input.points[0].z;
  float z2 = input.points[1].z;
  float z3 = input.points[2].z;

  float x1 = input.points[0].x;
  float x2 = input.points[1].x;
  float x3 = input.points[2].x;

  float u1 = input.texels[0].u;
  float u2 = input.texels[1].u;
  float u3 = input.texels[2].u;

  float v1 = input.texels[0].v;
  float v2 = input.texels[1].v;
  float v3 = input.texels[2].v;

  float w1 = input.texels[0].w;
  float w2 = input.texels[1].w;
  float w3 = input.texels[2].w;

		if (y2 < y1)
		{
			swap(y1, y2);
			swap(x1, x2);
			swap(u1, u2);
			swap(v1, v2);
			swap(w1, w2);
		}

		if (y3 < y1)
		{
			swap(y1, y3);
			swap(x1, x3);
			swap(u1, u3);
			swap(v1, v3);
			swap(w1, w3);
		}

		if (y3 < y2)
		{
			swap(y2, y3);
			swap(x2, x3);
			swap(u2, u3);
			swap(v2, v3);
			swap(w2, w3);
		}

		int dy1 = y2 - y1;
		int dx1 = x2 - x1;
		float dv1 = v2 - v1;
		float du1 = u2 - u1;
		float dw1 = w2 - w1;

		int dy2 = y3 - y1;
		int dx2 = x3 - x1;
		float dv2 = v3 - v1;
		float du2 = u3 - u1;
		float dw2 = w3 - w1;

		float tex_u, tex_v, tex_w;

		float dax_step = 0, dbx_step = 0,
			du1_step = 0, dv1_step = 0,
			du2_step = 0, dv2_step = 0,
			dw1_step=0, dw2_step=0;

		if (dy1) dax_step = dx1 / (float)abs(dy1);
		if (dy2) dbx_step = dx2 / (float)abs(dy2);

		if (dy1) du1_step = du1 / (float)abs(dy1);
		if (dy1) dv1_step = dv1 / (float)abs(dy1);
		if (dy1) dw1_step = dw1 / (float)abs(dy1);

		if (dy2) du2_step = du2 / (float)abs(dy2);
		if (dy2) dv2_step = dv2 / (float)abs(dy2);
		if (dy2) dw2_step = dw2 / (float)abs(dy2);

		if (dy1)
		{
			for (int i = y1; i <= y2; i++)
			{
				int ax = x1 + (float)(i - y1) * dax_step;
				int bx = x1 + (float)(i - y1) * dbx_step;

				float tex_su = u1 + (float)(i - y1) * du1_step;
				float tex_sv = v1 + (float)(i - y1) * dv1_step;
				float tex_sw = w1 + (float)(i - y1) * dw1_step;

				float tex_eu = u1 + (float)(i - y1) * du2_step;
				float tex_ev = v1 + (float)(i - y1) * dv2_step;
				float tex_ew = w1 + (float)(i - y1) * dw2_step;

				if (ax > bx)
				{
					swap(ax, bx);
					swap(tex_su, tex_eu);
					swap(tex_sv, tex_ev);
					swap(tex_sw, tex_ew);
				}

				tex_u = tex_su;
				tex_v = tex_sv;
				tex_w = tex_sw;

				float tstep = 1.0f / ((float)(bx - ax));
				float t = 0.0f;

				for (int j = ax; j < bx; j++)
				{
          if(tex_u / tex_w > 1.0 || tex_v / tex_w > 1.0)
            continue;
					tex_u = (1.0f - t) * tex_su + t * tex_eu;
					tex_v = (1.0f - t) * tex_sv + t * tex_ev;
					tex_w = (1.0f - t) * tex_sw + t * tex_ew;
          //J is X and I is Y
					if (tex_w > RI.depthBuffer[i*RI.engine->ScreenWidth() + j])
					{
            RI.engine->Draw(j, i, texture->Sample(tex_u / tex_w, tex_v / tex_w));
						RI.depthBuffer[i*RI.engine->ScreenWidth() + j] = tex_w;
					}
					t += tstep;
				}

			}
		}

		dy1 = y3 - y2;
		dx1 = x3 - x2;
		dv1 = v3 - v2;
		du1 = u3 - u2;
		dw1 = w3 - w2;

		if (dy1) dax_step = dx1 / (float)abs(dy1);
		if (dy2) dbx_step = dx2 / (float)abs(dy2);

		du1_step = 0, dv1_step = 0;
		if (dy1) du1_step = du1 / (float)abs(dy1);
		if (dy1) dv1_step = dv1 / (float)abs(dy1);
		if (dy1) dw1_step = dw1 / (float)abs(dy1);

		if (dy1)
		{
			for (int i = y2; i <= y3; i++)
			{
				int ax = x2 + (float)(i - y2) * dax_step;
				int bx = x1 + (float)(i - y1) * dbx_step;

				float tex_su = u2 + (float)(i - y2) * du1_step;
				float tex_sv = v2 + (float)(i - y2) * dv1_step;
				float tex_sw = w2 + (float)(i - y2) * dw1_step;

				float tex_eu = u1 + (float)(i - y1) * du2_step;
				float tex_ev = v1 + (float)(i - y1) * dv2_step;
				float tex_ew = w1 + (float)(i - y1) * dw2_step;

				if (ax > bx)
				{
					swap(ax, bx);
					swap(tex_su, tex_eu);
					swap(tex_sv, tex_ev);
					swap(tex_sw, tex_ew);
				}

				tex_u = tex_su;
				tex_v = tex_sv;
				tex_w = tex_sw;

				float tstep = 1.0f / ((float)(bx - ax));
				float t = 0.0f;

				for (int j = ax; j < bx; j++)
				{
          if(tex_u / tex_w > 1.0 || tex_v / tex_w > 1.0)
            continue;
					tex_u = (1.0f - t) * tex_su + t * tex_eu;
					tex_v = (1.0f - t) * tex_sv + t * tex_ev;
					tex_w = (1.0f - t) * tex_sw + t * tex_ew;

					if (tex_w > RI.depthBuffer[i*RI.engine->ScreenWidth() + j])
					{
            //J is X and I is Y
						RI.engine->Draw(j, i, texture->Sample(tex_u / tex_w, tex_v / tex_w));
						RI.depthBuffer[i*RI.engine->ScreenWidth() + j] = tex_w;
					}
					t += tstep;
				}
			}	
		}		
}

Matrix4x4 DoInputLoop(olc::PixelGameEngine* engine, Player* player)
{
  //Variable aliases for readibility
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
  Vector3D newLookDirection = LookDirection;
  Matrix4x4 cameraYRotationMatrix = GetRotationMatrix(ROT_TYPES::ROT_Y, fYaw * (mathPI / 180));

  //LookDirection is now updated in the Y direction
  MultiplyMatrixVector(TARGET, cameraYRotationMatrix, newLookDirection);

  //Set the facing vector to the new one
  player->camera.SetFacingVector(newLookDirection);
  TARGET = AddVector(CAMERA, newLookDirection);
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
          RI.engine->FillTriangle(point1, point2 , point3, triangleInput.color);
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
