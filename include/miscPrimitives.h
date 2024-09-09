#pragma once
#include "geometricPrimitives.h"
#include "olcPixelGameEngine.h"

class Light
{
private:
  short lightType;
  Vector3D direction;

public:
  float intensity = 1.0f;
  olc::Pixel color;
  Light(const short& lightType, const Vector3D& direction, const olc::Pixel& color = {255,255,255}, const float& intensity = 1.0f);
  void SetLightColor(const float& rVal, const float& gVal, const float& bVal);
  void SetLightDirection(const Vector3D& inputVec);
  bool SetLightType(const short& lampType);
  const Vector3D& GetDirection() const;
  const short& GetLightType() const;
};

class RenderingInstance
{
public:
  float* depthBuffer = nullptr;
  olc::PixelGameEngine* engine = nullptr; 
  void InitializeRenderingInstance(olc::PixelGameEngine* engine);
};

class Camera
{
public:
  Camera();
  Vector3D cameraPosition;
  void SetMovementSpeeds(const float& speedX, const float& speedY);
  void SetFacingVector(Vector3D facingVector);
  void SetFacingPlanes(const float& nearPlane, const float& farPlane);
  void SetFOV(const float& newFOV);
  void CalculateProjectionMatrix();
  const Matrix4x4 GetCameraProjectionMatrix() const;
  const Vector3D GetFacingVector() const;
  const pair<float, float> GetFacingPlanes() const;
  const float GetHorizontalSpeed() const;
  const float GetVerticalSpeed() const;
  const float GetFOV() const;
  float fYaw;
  float fPitch;

private:
  float speedHorizontal;
  float speedVertical;
  Vector3D facingVector;
  float nearPlane;
  float farPlane;
  float FOV;
  Matrix4x4 projectionMatrix;
};

//Simply a wrapper for a camera and other attributes
//Maybe health, damage, weapons, meshes, etc.
class Player
{
public:
  Camera camera;
  //Filler for other attributes
};
