#pragma once
#include <string>
#include <deque>
#include <array>
#include "olcPixelGameEngine.h"
#include "components.h"

#define BULK_COPY_ARRAY(src, dst, size) for(int i = 0; i < size; i++) dst[i] = src[i];

using namespace std;

class Vector2D
{
public:
  float u = 0.0f;
  float v = 0.0f;
  float w = 1.0f;
  const void PrintPoint() const;
  const string ExtractInfo() const;
};

class Vector3D
{
public:
  float x = 0.0f;
  float y = 0.0f; 
  float z = 0.0f;
  float w = 1.0f;
  void PrintPoint() const;
  string ExtractInfo() const;
};

class Triangle
{
public:
  array<Vector3D, 3> points;
  //TO-DO: Make this optionally assignable
  array<Vector2D, 3> texels;
  //std::span<Vector3D> p = points;
  //std::span<Vector2D> t = texels;
  olc::Pixel color;
  void PrintTriangle() const;
  string ExtractInfo() const;
};

class Mesh
{
public:
  bool doAutomaticRotation = false;
  bool isStatic = true;
  bool doLighting = true;
  bool doLines = false;
  bool hasTexture = false;
  //A dedicated struct encompassing material types might be added in the future
  //However, there are not enough material types to deserve this 
  uint8_t materialType;
  array<bool, 3> doAutomaticRotations = {false, false, false};
  Vector3D forwardVector = {0.0f, 0.0f, 1.0f};
  Vector3D lookAtVector;
  
  //All in X,Y,Z order
  array<float, 3> translationOffsets = {0.0f, 0.0f, 0.0f};
  array<float, 3> rotationSpeeds = {1.0f, 1.0f, 1.0f};
  array<float, 3> scalingOffsets = {1.0f, 1.0f, 1.0f};
  array<float, 3> rotationDegrees = {0.0f, 0.0f, 0.0f};

private:
  string meshName;
  string objFilePath;
  vector<Triangle> triangles;
  string* textureImagePath = nullptr;
  MeshComponents components;
  olc::Pixel* diffuseColor = nullptr;
  olc::Sprite* textureImageSprite = nullptr;
  olc::Decal* textureImageDecal = nullptr;

public:
  const int GetTotalVertices() const;
  const int GetTotalTriangles() const;
  const short GetMaterialType() const;
  const olc::Pixel* GetDiffuseColor() const;
  const vector<Triangle>& GetTriangles() const;
  const olc::Decal* GetTextureImage() const;
  const string* const GetTextureImagePath() const;
  const string& GetMeshName() const;
  const string& GetObjectFilePath() const;
  const MeshComponents& GetMeshComponents() const;

  void PrintTextureInformation() const;

  void SetTranslationOffsets(const float& newX, const float& newY, const float& newZ);
  void SetTranslationOffsets(const Vector3D& newTranslations);
  void SetRotationSpeeds(const float& newX, const float& newY, const float& newZ);
  void SetRotationSpeeds(const Vector3D& inputVector);
  void SetScalingOffsets(const float& newX, const float& newY, const float& newZ);
  void SetDiffuseColor(const uint8_t& rVal, const uint8_t& gVal, const uint8_t& bVal, const uint8_t& aVal);
  void SetDiffuseColor(const olc::Pixel& inputPixel);
  //The function returns whether the operation was successful or not
  bool SetTextureImage(const string& localPathToImage);
  bool SetMeshName(const string& newName);
  
  bool LoadFromOBJFile(const string& fileName, bool hasTexture = false);
  const void PrintMesh() const;
  const void PrintMeshInfo() const;
  const void PrintMeshToDisk(const string& fileName) const; //This is to verify if two meshes are equal

  Mesh* Duplicate();
  Mesh();
  Mesh(const std::string& name);
  ~Mesh();
};

class MeshList
{
//I am aware a deque has its disadvantages in large scenes and selections
//But I dont think the viewer or engine will scale to need such optimizations
private:
  deque<Mesh*> meshList;
  float totalTriangles;
  float totalVertices;

public:
  const int GetTotalVertices() const;
  const int GetTotalTriangles() const;
  const short GetMaterialType() const;
  deque<Mesh*>& GetMeshList();
  void UpdateTotalCounts();
  void AppendMesh(Mesh* input);
  bool LoadMeshFromOBJ(string filename);
};

class Matrix4x4
{
public:
  //All elements by default 0 unless specified
  array<array<float, 4>, 4> mat = {0};
  void PrintMatrix() const;
};

void PrintTrianglesToDisk(const vector<Triangle>& input,const string& fileName);
