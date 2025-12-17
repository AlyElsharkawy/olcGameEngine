#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include "serialization.h"

void SerializeMesh(const Mesh* const meshInput, const string& filePath)
{
  YAML::Node root;
  root["rotations"]["doAutomaticRotations"] = meshInput->doAutomaticRotations;
  root["rotations"]["doAutomaticRotation"] = meshInput->doAutomaticRotation;
  root["isStatic"] = meshInput->isStatic;
  root["doLighting"] = meshInput->doLighting;
  root["doLines"] = meshInput->doLines;
  root["materialType"] = static_cast<int>(meshInput->materialType);
  root["vectors"]["forwardVector"] = meshInput->forwardVector;
  root["vectors"]["lookAtVector"] = meshInput->lookAtVector;
  root["offsets"]["translationOffsets"] = meshInput->translationOffsets;
  root["offsets"]["rotationDegrees"] = meshInput->rotationDegrees;
  root["offsets"]["rotationSpeeds"] = meshInput->rotationSpeeds;
  root["offsets"]["scalingOffsets"] = meshInput->scalingOffsets;
  root["textureImagePath"] = (meshInput->GetTextureImagePath() == nullptr) ? 
    "" : *(meshInput->GetTextureImagePath());
  root["objectFilePath"] = meshInput->GetObjectFilePath();
  root["hasTexture"] = (meshInput->GetTextureImagePath() != nullptr) ? true : false;
  
  if(meshInput->GetDiffuseColor() != nullptr)
    root["diffuseColor"] = *(meshInput->GetDiffuseColor());

  ofstream outputMeshFile(filePath);
  outputMeshFile << root;
  outputMeshFile.close();
}

Mesh* DeserializeMesh(const string& filePath)
{
  typedef array<float, 3> tempType;
  Mesh* toReturn = new Mesh();
  YAML::Node root = YAML::LoadFile(filePath);
  toReturn->LoadFromOBJFile(root["objectFilePath"].as<string>(), root["hasTexture"].as<bool>());
 
  toReturn->doAutomaticRotation = root["rotations"]["doAutomaticRotation"].as<bool>();
  toReturn->doAutomaticRotations = root["rotations"]["doAutomaticRotations"].as<array<bool, 3>>();

  toReturn->isStatic = root["isStatic"].as<bool>();
  toReturn->doLighting = root["doLighting"].as<bool>();
  toReturn->doLines = root["doLines"].as<bool>();
  toReturn->materialType = root["materialType"].as<uint8_t>();

  toReturn->SetTranslationOffsets(root["offsets"]["translationOffsets"].as<Vector3D>());
  toReturn->SetRotationSpeeds(root["offsets"]["rotationSpeeds"].as<Vector3D>());
  toReturn->rotationDegrees = root["offsets"]["rotationDegrees"].as<array<float, 3>>();
  toReturn->scalingOffsets = root["offsets"]["scalingOffsets"].as<array<float, 3>>();

  toReturn->lookAtVector = root["vectors"]["lookAtVector"].as<Vector3D>();
  toReturn->forwardVector = root["vectors"]["forwardVector"].as<Vector3D>();

  if(root["diffuseColor"])
    toReturn->SetDiffuseColor(root["diffuseColor"].as<olc::Pixel>());

  if(root["textureImagePath"].as<string>().empty() == false)
    toReturn->SetTextureImage(root["textureImagePath"].as<string>());

  return toReturn;
}
