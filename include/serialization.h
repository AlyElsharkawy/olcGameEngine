#pragma once
#include <string>
#include <array>
#include "miscPrimitives.h"
#include "yaml-cpp/yaml.h"

void SerializeLight(const Light* const lightInput, const string& filePath);
Light* DeserializeLight(const string& filePath); 

void SerializeMesh(const Mesh* const meshInput, const string& filePath);
Mesh* DeserializeMesh(const string& filePath);

namespace YAML 
{
  template<>
  struct convert<olc::Pixel> 
  {
    static Node encode(const olc::Pixel& pixelInput)
    {
        Node node;
        node["r"] = static_cast<int>(pixelInput.r);
        node["g"] = static_cast<int>(pixelInput.g);
        node["b"] = static_cast<int>(pixelInput.b);
        return node;
    }

    static bool decode(const Node& node, olc::Pixel& pixelOutput) 
    {
      if(!node.IsMap()) 
        return false;
      pixelOutput.r = node["r"].as<uint8_t>();
      pixelOutput.g = node["g"].as<uint8_t>();
      pixelOutput.b = node["b"].as<uint8_t>();
      return true;
    }
  };

  template<>
  struct convert<Vector3D>
  {
    static Node encode(const Vector3D& vectorInput)
    {
      Node node;
      node["x"] = vectorInput.x;
      node["y"] = vectorInput.y;
      node["z"] = vectorInput.z;
      return node;
    }

    static bool decode(const Node& node, Vector3D& vectorOutput)
    {
      if(!node.IsMap()) 
        return false;
      vectorOutput.x = node["x"].as<float>();
      vectorOutput.y = node["y"].as<float>();
      vectorOutput.z = node["z"].as<float>();
      return true;
    }
  };

  template<typename T>
  struct convert<array<T, 3>>
  {
    static Node encode(const array<T, 3>& arrayInput)
    {
      Node node;
      node["x"] = arrayInput[0];
      node["y"] = arrayInput[1];
      node["z"] = arrayInput[2];
      return node;
    }

    static bool decode(const Node& node, array<T, 3>& arrayOutput)
    {
      if(!node.IsMap())
        return false;
      arrayOutput[0]  = node["x"].as<T>();
      arrayOutput[1]  = node["y"].as<T>();
      arrayOutput[2]  = node["z"].as<T>();
      return true;
    }
  };
}
