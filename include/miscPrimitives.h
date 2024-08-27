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
  float* screenBuffer = nullptr;
  olc::PixelGameEngine* engine = nullptr; 
  void InitializeRenderingInstance(olc::PixelGameEngine* engine);
};
