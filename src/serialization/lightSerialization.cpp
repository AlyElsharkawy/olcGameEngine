#include <yaml-cpp/yaml.h>
#include "miscPrimitives.h"
#include "serialization.h"

void SerializeLight(const Light* const lightInput, const string& filePath)
{
  YAML::Node root;
  const olc::Pixel& color = lightInput->color;
  
  root["color"] = lightInput->color;
  root["intensity"] = lightInput->intensity;
  root["direction"] = lightInput->GetDirection();
  root["type"] = lightInput->GetLightType();
  root["position"] = lightInput->GetPosition();
  root["name"] = lightInput->GetLightName();

  //Add later when mesh serialization is added
  if(lightInput->GetMeshComponent() != nullptr)
  {
    //root["components"]["mesh"] = 
  }

  ofstream lightOutputFile(filePath);
  lightOutputFile << root;
  lightOutputFile.close();
}

Light* DeserializeLight(const string& filePath)
{
  YAML::Node root = YAML::LoadFile(filePath);
  olc::Pixel color;
  float intensity;
  string lightName;
  uint8_t type;
  try
  {
    color = root["color"].as<olc::Pixel>();
    intensity = root["intensity"].as<float>();
    type = root["type"].as<uint8_t>();
    lightName = root["name"].as<string>();
  }
  catch(const exception& e)
  {
    cerr << "Failed to parse light color and intensity, light type, or name information for " << filePath << '\n';
    return nullptr;
  }

  Vector3D direction, position;
  try
  {
    direction = root["direction"].as<Vector3D>();
    position = root["position"].as<Vector3D>();
  }
  catch(const exception& e)
  {
    cerr << "Failed to parse direction and position information for " << filePath << '\n';
  }

  if(root["components"])
  {
    try
    {
    }
    catch(const exception& e)
    {
    }
  }
  Light* toReturn = new Light(type, direction, position, lightName, color, intensity);
  return toReturn;
}
