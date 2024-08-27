#include "miscPrimitives.h"
#include "globalVariables.h"

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
    case LIGHT_TYPES::SUN:
      {
        this->lightType = LIGHT_TYPES::SUN;
        break;
      }
    case LIGHT_TYPES::POINT:
      {
        this->lightType = LIGHT_TYPES::POINT;
        break;
      }
    default:
      {
        cerr << "Invalid lamp type specified. Defaulted to sun.\n";
        this->lightType = LIGHT_TYPES::SUN;
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
  this->screenBuffer = new float[engine->ScreenHeight() * engine->ScreenHeight()];
}
