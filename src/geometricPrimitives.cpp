#include <filesystem>
#include <iostream>
#include <strstream>
#include <fstream>
#include <vector>
#include <string>
#include "geometricPrimitives.h"
#include "globalVariables.h"
#include "miscFunctions.h"

using namespace std;

const string Vector2D::ExtractInfo() const
{
  return "U: " + to_string(this->u) + ", V: " + to_string(this->v);
}

const void Vector2D::PrintPoint() const
{
  cout << "U: " << this->u << "V: " << this->v << '\n';
}

void Vector3D::PrintPoint() const
{
  cout << "X: " << this->x << " Y: " << this->y << " Z: " << this->z << "\n";
}

string Vector3D::ExtractInfo() const
{
  return "X: " + to_string(this->x) + ", Y: " + to_string(this->y) + ", Z: " + to_string(this->z);
}

void Triangle::PrintTriangle() const
{
  cout << "Triangle: \n";
  for(int i = 0; i < 3; i++)
  {
    this->points[i].PrintPoint();
  }
  cout << "Color\t";
  cout << "R: " << to_string(this->color.r) << " G: " << to_string(this->color.g) << " B: " << to_string(this->color.b) << "\n";
}

string Triangle::ExtractInfo() const
{
  return '{' + this->points[0].ExtractInfo() + ", " +
               this->points[1].ExtractInfo() + ", " + 
               this->points[2].ExtractInfo() + ", " +
               this->texels[0].ExtractInfo() + ", " +
               this->texels[1].ExtractInfo();
}

void Matrix4x4::PrintMatrix() const
{
  for(int i = 0; i < 4; i++)
  {
    for(int j = 0; j < 4; j++)
    {
      cout << this->mat[i][j] << ' ';
    }
    cout << '\n';
  }
}

const int Mesh::GetTotalVertices() const
{
  return this->totalVertices;
}

const int Mesh::GetTotalVisibleVertices() const
{
  return this->visibleVertices;
}

const int Mesh::GetTotalTriangles() const
{
  return this->totalTriangles;
}

const int Mesh::GetTotalVisibleTriangles() const
{
  return this->visibleTriangles;
}

const short Mesh::GetMaterialType() const
{
  return this->materialType;
}

const olc::Pixel* Mesh::GetDiffuseColor() const
{
  return this->diffuseColor;
}
  
const olc::Decal* Mesh::GetTextureImage() const
{
  return this->textureImage;
}

const olc::Decal* Mesh::GetNormalImage() const
{
  return this->normalImage;
}

void Mesh::PrintTextureInformation() const
{
  if(this->normalImage != nullptr)
  {
    olc::Sprite* normalSprite = this->normalImage->sprite;
    cout << "Normal Texture Information:\n";
    cout << "Image size: " << normalSprite->Size() << '\n';
    cout << "Image Path: " << this->normalImagePath << '\n';
  }
  if(this->textureImage != nullptr)
  {
    olc::Sprite* textureSprite = this->textureImage->sprite;
    cout << "Image Texture Information:\n";
    cout << "Image size: " << textureSprite->Size() << '\n';
    cout << "Image Path: " << this->textureImagePath << '\n';
  }
}

void Mesh::SetVisibleTriangles(const int& visibleTrianglesCount)
{
  this->visibleTriangles = visibleTrianglesCount;
  this->visibleVertices = visibleTrianglesCount * 3;
}

const void Mesh::PrintMesh() const
{
  cout << "Mesh:\n";
  for(int i = 0; i < this->triangles.size(); i++)
  {
    this->triangles[i].PrintTriangle();
  }
  cout << "End of mesh\n";
}

void Mesh::SetTranslationOffsets(const float& newX, const float& newY, const float& newZ)
{
  this->translationOffsets[0] = newX;
  this->translationOffsets[1] = newY;
  this->translationOffsets[2] = newZ;
}

void Mesh::SetRotationSpeeds(const float& newX, const float& newY, const float& newZ)
{
  this->rotationSpeeds[0] = newX;
  this->rotationSpeeds[1] = newY;
  this->rotationSpeeds[2] = newZ;
}

void Mesh::SetScalingOffsets(const float& newX, const float& newY, const float& newZ)
{
  this->scalingOffsets[0] = newX;
  this->scalingOffsets[1] = newY;
  this->scalingOffsets[2] = newZ;
}

void Mesh::SetDiffuseColor(const uint8_t& rVal, const uint8_t& gVal, const uint8_t& bVal, const uint8_t& aVal)
{
  this->diffuseColor = new olc::Pixel(rVal, gVal, bVal, aVal);
  this->materialType = MATERIAL_TYPES::DIFFUSE;
}

bool Mesh::SetTextureImage(const string& pathToImage)
{
  this->materialType = MATERIAL_TYPES::TEXTURE;
  if(filesystem::exists(pathToImage))
  {
    olc::Sprite* temp = new olc::Sprite(pathToImage);
    this->textureImage = new olc::Decal(temp);
    this->textureImagePath = pathToImage;
    return true;
  }

  else
  {
    string missingTexturePath = GetPathFromResources({"textures", "missingTexture.png"});
    olc::Sprite* temp = new olc::Sprite(missingTexturePath);
    this->textureImage = new olc::Decal(temp);
    return false;
  }
}

bool Mesh::SetNormalImage(const string& pathToImage)
{
  this->materialType = MATERIAL_TYPES::TEXTURE_WITH_NORMAL;
  if(filesystem::exists(pathToImage))
  {
    olc::Sprite* temp = new olc::Sprite(pathToImage);
    this->normalImage = new olc::Decal(temp);
    this->normalImagePath = pathToImage;
    return true;
  }
  else
    return false;
}

bool Mesh::LoadFromOBJFile(const string& fileName, bool hasTexture)
{
  ifstream inputFile(fileName, std::ios::in);
  vector<Triangle> triangles; 
  vector<Vector3D> vertices;
  vector<Vector2D> textels;
  int lineCounter = 1;
  if(!inputFile.is_open())
  {
    cerr << "ERROR: Can not open " << fileName << ". Please check file and try again!\n";
    return false;
  }
  while(!inputFile.eof())
  {   
    char lineBuffer[128];
    char junkChar;
    cout << "Currently on line: " << lineCounter << '\n';
    inputFile.getline(lineBuffer, 128);
    strstream stringStream;
    stringStream << lineBuffer;

    if(lineBuffer[0] == 'v')
    {
      if(lineBuffer[1] == 't')
      {
        Vector2D tempTextel;
        stringStream >> junkChar >> junkChar >> tempTextel.u >> tempTextel.v;
        textels.push_back(tempTextel);
      }
      else
      {
        Vector3D tempVector;
        stringStream >> junkChar >> tempVector.x >> tempVector.y >> tempVector.z;
        vertices.push_back(tempVector);
      }
    }
    
    if(hasTexture == false)
    {
      if(lineBuffer[0] == 'f')
      {
        int faces[3];
        stringStream >> junkChar >> faces[0] >> faces[1] >> faces[2];
        triangles.push_back({vertices[faces[0] - 1], vertices[faces[1] - 1], vertices[faces[2] - 1]});
      }
    }
    
    if(hasTexture == true)
    {
			if (lineBuffer[0] == 'f')
			{
				stringStream >> junkChar;

				string tokens[6];
				int nTokenCount = -1;


				while (!stringStream.eof())
				{
					char c = stringStream.get();
					if (c == ' ' || c == '/')
						nTokenCount++;
					else
						tokens[nTokenCount].append(1, c);
				}

				tokens[nTokenCount].pop_back();

				triangles.push_back({ vertices[stoi(tokens[0]) - 1], vertices[stoi(tokens[2]) - 1], vertices[stoi(tokens[4]) - 1],
		    textels[stoi(tokens[1]) - 1], textels[stoi(tokens[3]) - 1], textels[stoi(tokens[5]) - 1] });
      }
    }
    lineCounter++;
  }
  this->triangles = triangles;
  this->totalTriangles = triangles.size();
  return true;
}

Mesh::Mesh()
{
  this->materialType = MATERIAL_TYPES::NONE;
}

const int MeshList::GetTotalTriangles() const
{
  return this->totalTriangles;
}

const int MeshList::GetTotalVisibleTriangles() const
{
  return this->visibleTriangles;
}

const int MeshList::GetTotalVertices() const
{
  return this->totalVertices;
}

const int MeshList::GetTotalVisibleVertices() const
{
  return this->visibleTriangles;
}

deque<Mesh>& MeshList::GetMeshList()
{
  return this->meshList;
}

void MeshList::UpdateVisibleCounts()
{
  int newVisibleTriangles = 0;
  for(int i = 0; i < this->meshList.size(); i++)
    newVisibleTriangles += this->meshList[i].GetTotalVisibleTriangles();
  this->visibleTriangles = newVisibleTriangles;
  this->visibleVertices = newVisibleTriangles * 3;
}

void MeshList::UpdateTotalCounts()
{
  int newTotalTriangles = 0;
  for(int i = 0; i < this->meshList.size(); i++)
    newTotalTriangles += meshList[i].GetTotalTriangles();
  this->totalTriangles = newTotalTriangles;
  this->totalVertices = newTotalTriangles * 3;
}

//Consideration: performance of std::move
void MeshList::AppendMesh(Mesh& input)
{
  this->meshList.emplace_back(std::move(input));
}

bool MeshList::LoadMeshFromOBJ(string fileName)
{
  Mesh temp;
  if(temp.LoadFromOBJFile(fileName) == false)
    return false;
  this->meshList.emplace_back(std::move(temp));
  return true;
}
