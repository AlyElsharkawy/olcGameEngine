#pragma once
#include "components.h"
#include "geometricPrimitives.h"
#include "olcPixelGameEngine.h"

class NormalizedPixel
{
public:
  float values[3];
  NormalizedPixel(const float& r, const float& g, const float& b);
};

class Light
{
private:
  string lightName;
  LightComponents components;
  Vector3D direction;
  Vector3D position;
  short lightType;

public:
  float intensity = 1.0f;
  olc::Pixel color;
  float normalizedColors[3];
  Light(const short& lightType, const Vector3D& direction, const Vector3D& position,
        const string& lightName, const olc::Pixel& color = {255,255,255}, const float& intensity = 1.0f);
  ~Light();
  void SetLightColor(const float& rVal, const float& gVal, const float& bVal);
  void SetLightDirection(const Vector3D& inputVec);
  bool SetLightType(const short& lampType);
  void AddMeshComponent(Mesh* inputMesh);
  void MoveLight(const Vector3D& newPosition);
  void ChangeDirection(const Vector3D& newDirection);
  const Vector3D& GetPosition() const;
  const Vector3D& GetDirection() const;
  const short& GetLightType() const;
  const Mesh* const GetMeshComponent() const;
  //R,G,B
  const float* const GetNormalizedColorCodes() const;
  const string& GetLightName() const;
  void PrintLightInfo() const;
};

class RenderingInstance
{
public:
  float* depthBuffer = nullptr;
  olc::PixelGameEngine* engine = nullptr; 
  Mesh* selectedMesh;
  void InitializeRenderingInstance(olc::PixelGameEngine* engine);
  ~RenderingInstance();
  //void SetProjectionMatrix(const Matrix4x4& matrixInput);
};

class Camera
{
public:
  Camera();
  Vector3D cameraPosition;
  void SetMovementSpeeds(const float& speedX, const float& speedY, const float& speedRotation);
  void SetFacingVector(Vector3D facingVector);
  void SetFacingPlanes(const float& nearPlane, const float& farPlane);
  void SetFOV(const float& newFOV);
  void CalculateProjectionMatrix();
  const Matrix4x4& GetCameraProjectionMatrix() const;
  const Vector3D& GetFacingVector() const;
  const pair<float, float> GetFacingPlanes() const;
  const float GetHorizontalSpeed() const;
  const float GetVerticalSpeed() const;
  const float GetRotationSpeed() const;
  const float GetFOV() const;
  float fYaw;
  float fPitch;

private:
  float speedHorizontal;
  float speedVertical;
  float speedRotation;
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
