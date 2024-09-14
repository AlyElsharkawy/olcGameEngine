#include "miscPrimitives.h"
#include "geometricPrimitives.h"
#include "matrixMathEssentials.h"
#include "vectorMathEssentials.h"
#include "globalVariables.h"
#include <iostream>

Light::Light(const short& lightType, const Vector3D& direction, const olc::Pixel& color, const float& intensity)
{
  bool isLightTypeValid = this->SetLightType(lightType);
  if(isLightTypeValid == false)
    return;
  this->SetLightDirection(direction);
  this->color = color;
  this->intensity = intensity;
}
void Light::SetLightColor(const float& rVal, const float& gVal, const float& bVal)
{
  this->color = olc::Pixel(rVal, gVal, bVal, 255);
}

void Light::SetLightDirection(const Vector3D& inputVec)
{
  Vector3D temp = inputVec;
  //Due to strange linker error
  float length = sqrtf(inputVec.x * inputVec.x + inputVec.y * inputVec.y  + inputVec.z * inputVec.z);
  temp.x /= length; temp.y /= length; temp.z /= length;

  this->direction = temp;
}

bool Light::SetLightType(const short& lampType)
{
  switch(lampType)
  {
    case LIGHT_TYPES::LAMP_SUN:
      {
        this->lightType = LIGHT_TYPES::LAMP_SUN;
        break;
      }
    case LIGHT_TYPES::LAMP_POINT:
      {
        this->lightType = LIGHT_TYPES::LAMP_POINT;
        break;
      }
    default:
      {
        cerr << "Invalid lamp type specified. Defaulted to sun.\n";
        this->lightType = LIGHT_TYPES::LAMP_SUN;
        return false;
      }
  }
  return true;
}

const Vector3D& Light::GetDirection() const
{
  return this->direction;
}

const short& Light::GetLightType() const
{
  return this->lightType;
}

void RenderingInstance::InitializeRenderingInstance(olc::PixelGameEngine* engine)
{
  this->engine = engine;
  this->depthBuffer = new float[engine->ScreenWidth() * engine->ScreenHeight()];
}

void RenderingInstance::SetProjectionMatrix(const Matrix4x4 matrixInput)
{
  this->PROJECTION_MATRIX = new Matrix4x4();
  for(int i = 0; i < 4; i++)
  {
    for(int j = 0; j < 4; j++)
    {
      this->PROJECTION_MATRIX->mat[i][j] = matrixInput.mat[i][j];
    }
  }
}

Camera::Camera()
{
  this->speedVertical = MINIMUM_VERTICAL_SPEED; this->speedHorizontal = MINIMUM_HORIZONTAL_SPEED;
  this->nearPlane = MINIMUM_NEAR_DISTANCE; this->farPlane = MINIMUM_FAR_DISTANCE;
  this->facingVector = DEFAULT_FACING_VECTOR;
  this->FOV = DEFAULT_FOV;
}

void Camera::SetMovementSpeeds(const float& speedX, const float& speedY, const float& speedRotation)
{
  if(speedX > 0.0f)
  {
    this->speedHorizontal = speedX;
  }
  else
  {
    this->speedHorizontal = MINIMUM_HORIZONTAL_SPEED;
    cerr << "Error: Camera horizontal speed cannot be negative\n";
  }
  
  if(speedY > 0.0f)
  {
    this->speedVertical = speedY;
  }
  else
  {
    this->speedVertical = MINIMUM_VERTICAL_SPEED;
    cerr << "Error: Camera vertical speed cannot be negative\n";
  }

  if(speedRotation > 0.0f)
  {
    this->speedRotation = speedRotation;
  }
  
  else
  {
    this->speedRotation = MINIMUM_ROTATION_SPEED;
    cerr << "Error: Camera rotation speed cannot be negative\n";
  }
}

void Camera::SetFacingVector(Vector3D facingVector)
{
  NormalizeVector(facingVector);
  this->facingVector = facingVector;
}

void Camera::SetFacingPlanes(const float& nearPlane, const float& farPlane)
{
  if(nearPlane > 0.0f)
  {
    this->nearPlane = nearPlane;
  }
  else
  {
    cerr << "Error: Camera near planes cannot be negative\n";
    this->nearPlane = MINIMUM_NEAR_DISTANCE;
  }

  if(farPlane > 0.0f && farPlane > this->nearPlane)
  {
    this->farPlane = nearPlane;
  }
  else
  {
    this->farPlane = MINIMUM_FAR_DISTANCE;
    cerr << "Error: Camera far distance value cannot be negative and/or smaller than the near plane value\n";
  }
}

void Camera::CalculateProjectionMatrix()
{
  float fovRadians = 1.0f / tanf(this->FOV * 0.5 / 180.0f * mathPI);
  this->projectionMatrix = GetProjectionMatrix(ASPECT_RATIO, fovRadians, this->nearPlane, this->farPlane);
}

void Camera::SetFOV(const float& newFOV)
{
  if(newFOV > 0.0f)
  {
    this->FOV = newFOV;
  }
  else
  {
    cerr << "Error: Camera FOV cannot be negative\n";
    this->FOV = DEFAULT_FOV;
  }
}

const Matrix4x4& Camera::GetCameraProjectionMatrix() const
{
  return this->projectionMatrix;
}

const Vector3D& Camera::GetFacingVector() const
{
  return this->facingVector;
}

const pair<float, float> Camera::GetFacingPlanes() const
{
  return make_pair(this->nearPlane, this->farPlane);
}

const float Camera::GetHorizontalSpeed() const
{
  return this->speedHorizontal;
}

const float Camera::GetVerticalSpeed() const
{
  return this->speedVertical;
}

const float Camera::GetRotationSpeed() const
{
  return this->speedRotation;
}
const float Camera::GetFOV() const
{
  return this->FOV;
}
