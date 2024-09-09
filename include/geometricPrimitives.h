#pragma once
#include <string>
#include <deque>
#include "olcPixelGameEngine.h"

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
  Vector3D points[3];
  //TO-DO: Make this optionally assignable
  Vector2D texels[3];
  olc::Pixel color;
  void PrintTriangle() const;
  string ExtractInfo() const;
};

class Mesh
{
public:
  vector<Triangle> triangles;
  bool doAutomaticRotation = false;
  bool doZRotation = false;
  bool doYRotation = false;
  bool doXRotation = false;
  bool isStatic = true;
  Vector3D forwardVector = {0.0f,0.0f,1.0f};
  //A dedicated struct encompassing material types might be added in the future
  //However, there are not enough material types to deserve this 
  Vector3D lookAtVector;
  float fTheta;
  //In X,Y,Z order
  float translationOffsets[3] = {0.0f};
  //In X,Y,Z order
  float rotationSpeeds[3] = {1.0f,1.0f,1.0f};
  //In X,Y,Z order
  float scalingOffsets[3] = {1.0f,1.0f,1.0f};
  //In X,Y,Z order 
  float rotationDegrees[3] = {0.0f};

private:
  float totalTriangles;
  float visibleTriangles;
  float totalVertices;
  float visibleVertices;
  short materialType;
  olc::Pixel* diffuseColor = nullptr;
  olc::Sprite* textureImageSprite = nullptr;
  olc::Sprite* normalImageSprite = nullptr;
  olc::Decal* textureImageDecal = nullptr;
  olc::Decal* normalImageDecal = nullptr;

public:
  const int GetTotalVertices() const;
  const int GetTotalVisibleVertices() const;
  const int GetTotalTriangles() const;
  const int GetTotalVisibleTriangles() const;
  const short GetMaterialType() const;
  const olc::Pixel* GetDiffuseColor() const;
  string textureImagePath;
  string normalImagePath;
  const olc::Decal* GetTextureImage() const;
  const olc::Decal* GetNormalImage() const;

  void PrintTextureInformation() const;

  void SetVisibleTriangles(const int& visibleTrianglesCount);
  void SetTranslationOffsets(const float& newX, const float& newY, const float& newZ);
  void SetRotationSpeeds(const float& newX, const float& newY, const float& newZ);
  void SetScalingOffsets(const float& newX, const float& newY, const float& newZ);
  void SetDiffuseColor(const uint8_t& rVal, const uint8_t& gVal, const uint8_t& bVal, const uint8_t& aVal);
  //The 2 bottom functions return whether the operation was succesful or not
  bool SetTextureImage(const string& localPathToImage);
  bool SetNormalImage(const string& localPathToImage);
  
  bool LoadFromOBJFile(const string& fileName, bool hasTexture = false);
  const void PrintMesh() const;
  Mesh();
  ~Mesh();
};

class MeshList
{
//I am aware a deque has its disadvantages in large scenes and selections
//But I dont think the viewer or engine will scale to need such optimizations
private:
  deque<Mesh*> meshList;
  float totalTriangles;
  float visibleTriangles;
  float totalVertices;
  float visibleVertices;

public:
  const int GetTotalVertices() const;
  const int GetTotalVisibleVertices() const;
  const int GetTotalTriangles() const;
  const int GetTotalVisibleTriangles() const;
  const short GetMaterialType() const;
  deque<Mesh*>& GetMeshList();
  void UpdateVisibleCounts();
  void UpdateTotalCounts();
  void AppendMesh(Mesh* input);
  bool LoadMeshFromOBJ(string filename);
};

class Matrix4x4
{
public:
  //All elements by default 0 unless specified
  float mat[4][4] = {0};
  void PrintMatrix() const;
};
