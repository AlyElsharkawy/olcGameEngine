#include <exception>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include <filesystem>
#include "serialization.h"

void SerializeMesh(const Mesh* const meshInput, const string& meshFilePath, 
                   const string& objectFilePath, const string& textureImagePath)
{
  YAML::Node root;
  root["rotations"]["doAutomaticRotations"] = meshInput->doAutomaticRotations;
  root["rotations"]["doAutomaticRotation"] = meshInput->doAutomaticRotation;
  root["isStatic"] = meshInput->isStatic;
  root["doLighting"] = meshInput->doLighting;
  root["doLines"] = meshInput->doLines;
  root["meshName"] = meshInput->GetMeshName();
  root["materialType"] = static_cast<int>(meshInput->materialType);
  root["vectors"]["forwardVector"] = meshInput->forwardVector;
  root["vectors"]["lookAtVector"] = meshInput->lookAtVector;
  root["offsets"]["translationOffsets"] = meshInput->translationOffsets;
  root["offsets"]["rotationDegrees"] = meshInput->rotationDegrees;
  root["offsets"]["rotationSpeeds"] = meshInput->rotationSpeeds;
  root["offsets"]["scalingOffsets"] = meshInput->scalingOffsets;
  
  root["textureImagePath"] = (meshInput->GetTextureImagePath() == nullptr) ? 
    "" : textureImagePath;
  if(meshInput->GetTextureImagePath() != nullptr)
  {
    try
    {
      std::filesystem::copy_file(*(meshInput->GetTextureImagePath()), 
                                std::filesystem::canonical(meshFilePath + "/../" + textureImagePath));
    }
    catch(const std::exception& e)
    {
      cerr << "Serialization failed for " << meshInput->GetMeshName() <<" .Failed to copy texture image png file " 
      << *(meshInput->GetTextureImagePath()) << " during deserialization.\n";
    }
  }

  root["objectFilePath"] = objectFilePath; //RELATIVE PATH
  root["hasTexture"] = meshInput->hasTexture;
  
  if(meshInput->GetDiffuseColor() != nullptr)
    root["diffuseColor"] = *(meshInput->GetDiffuseColor());

  try
  {
    std::filesystem::copy_file(meshInput->GetObjectFilePath(), 
                               std::filesystem::canonical(meshFilePath + ".." + objectFilePath));
  }
  catch(const std::exception& e)
  {
    cerr << "Serialization failed for " << meshInput->GetMeshName() <<" .Failed to copy OBJ file " 
      << meshInput->GetObjectFilePath() << " during deserialization.\n";
    return;
  }

  if(meshInput->GetMeshComponents().light != nullptr)
  {
    std::filesystem::create_directory(std::filesystem::canonical(meshFilePath + "../light"));
    const std::string& meshName = meshInput->GetMeshName();
    SerializeLight(meshInput->GetMeshComponents().light, meshFilePath + "/light/" + meshName + ".light");
  }

  ofstream outputMeshFile(meshFilePath);
  outputMeshFile << root;
  outputMeshFile.close();
}

Mesh* DeserializeMesh(const string& filePath)
{
  typedef array<float, 3> tempType;
  Mesh* toReturn = new Mesh();
  YAML::Node root = YAML::LoadFile(filePath);
  try
  {
    toReturn->LoadFromOBJFile(std::filesystem::canonical(root["objectFilePath"].as<string>()), 
                            root["hasTexture"].as<bool>());
  }
  catch(const std::exception& e)
  {
    cerr << "Deserialization failed for " << root["meshName"].as<string>() << '\n';
    cerr << "Error: " << e.what() << '\n';
    delete toReturn;
    return nullptr;
  }
 
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
