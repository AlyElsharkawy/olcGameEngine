#include <filesystem>
#include <iostream>
#include <strstream>
#include <fstream>
#include <vector>
#include <string>
#include "geometricPrimitives.h"
#include "essentialFunctions.h"
#include "globalVariables.h"
#include "miscFunctions.h"

using namespace std;

const string Vector2D::ExtractInfo() const
{
  return "U: " + to_string(this->u) + ", V: " + to_string(this->v) + ", W: " + to_string(this->w);
}

const void Vector2D::PrintPoint() const
{
  cout << "U: " << this->u << " V: " << this->v << " W: " << this->w << '\n';
}

void Vector3D::PrintPoint() const
{
  cout << "X: " << this->x << " Y: " << this->y << " Z: " << this->z << " W: " << this->w << '\n';
}

string Vector3D::ExtractInfo() const
{
  return "X: " + to_string(this->x) + ", Y: " + to_string(this->y) + ", Z: " + to_string(this->z) + ", W: " + to_string(this->w);
}

void Triangle::PrintTriangle() const
{
  cout << "Triangle: \n";
  for(int i = 0; i < 3; i++)
  {
    this->points[i].PrintPoint();
  }
  cout << "Triangle Textels: \n";
  for(int i = 0; i < 3; i++)
  {
    this->texels[i].PrintPoint();
  }
  cout << "Color->\t";
  cout << "R: " << to_string(this->color.r) << " G: " << to_string(this->color.g) << " B: " << to_string(this->color.b) << "\n";
}

string Triangle::ExtractInfo() const
{
  /*return '{' + this->points[0].ExtractInfo() + ", " +
               this->points[1].ExtractInfo() + ", " + 
               this->points[2].ExtractInfo() + ", " +
               this->texels[0].ExtractInfo() + ", " +
               this->texels[1].ExtractInfo();*/

  string result =  "Triangle: \n";
  for(int i = 0; i < 3; i++)
  {
    result += this->points[i].ExtractInfo() + '\n';
  }
  result += "Triangle Textels: \n";
  for(int i = 0; i < 3; i++)
  {
    result += this->texels[i].ExtractInfo() + '\n';
  }
  result += "Color->\t";
  result += "R: " + to_string(this->color.r) + " G: " + to_string(this->color.g) + " B: " + to_string(this->color.b) + "\n";
  return result;
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
  return this->triangles.size() * 3;
}


const int Mesh::GetTotalTriangles() const
{
  return this->triangles.size();
}

const short Mesh::GetMaterialType() const
{
  return this->materialType;
}

const olc::Pixel* Mesh::GetDiffuseColor() const
{
  return this->diffuseColor;
}

const vector<Triangle>& Mesh::GetTriangles() const
{
  return this->triangles;
}
  
const olc::Decal* Mesh::GetTextureImage() const
{
  return this->textureImageDecal;
}

const string* const Mesh::GetTextureImagePath() const
{
  return this->textureImagePath;
}

const string& Mesh::GetMeshName() const
{
  return this->meshName;
}

void Mesh::PrintTextureInformation() const
{
  if(this->textureImageSprite != nullptr)
  {
    cout << "Image Texture Information:\n";
    cout << "Image size: " << this->textureImageSprite->Size() << '\n';
    cout << "Image Path: " << this->textureImagePath << '\n';
  }
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

#define INT_TO_BOOL(var) (var == true ? "true" : "false")
#define PRINT_ELEMENTS_INLINE(arr) \
for(const auto& elm : arr) \
  cout << elm << ' '; \
cout << '\n';

const void Mesh::PrintMeshInfo() const
{
  cout << "Mesh name: " << this->meshName << '\n';
  cout << "Forward Vector -> " << this->forwardVector.ExtractInfo() << '\n';
  cout << "LookAt Vector -> " << this->lookAtVector.ExtractInfo() << '\n';
  cout << "Diffuse color -> " << INT_TO_BOOL(this->diffuseColor == nullptr) << '\n';
  if(this->diffuseColor != nullptr)
  {
    olc::Pixel& temp = *this->diffuseColor;
    cout <<
    "R: " << to_string(temp.r) << 
    " G: " << to_string(temp.g) <<
    " B: " << to_string(temp.b) << '\n';
  }
  cout << "Has Texture -> " << INT_TO_BOOL(this->hasTexture) << '\n';
  if(this->hasTexture == true)
  {
    PrintTextureInformation();
  }
  cout << "Is static -> " << INT_TO_BOOL(this->isStatic) << '\n';
  cout << "Do Lines -> " << INT_TO_BOOL(this->doLines) << '\n';
  cout << "Do Lighting -> " << INT_TO_BOOL(this->doLighting) << '\n';
  cout << "Material Type: " << to_string(this->materialType) << '\n';
  cout << "Translation Offsets (X, Y, Z) -> ";
  PRINT_ELEMENTS_INLINE(this->translationOffsets);
  cout << "Rotation Degrees (X, Y, Z) -> ";
  PRINT_ELEMENTS_INLINE(this->rotationDegrees);
  cout << "Scaling Offsets (X, Y, Z) -> ";
  PRINT_ELEMENTS_INLINE(this->rotationDegrees);
  cout << "Rotation Speeds (X, Y , Z) -> ";
  PRINT_ELEMENTS_INLINE(this->rotationSpeeds);
  cout << '\n';
}

const void Mesh::PrintMeshToDisk(const string& fileName) const
{
  ofstream outputFile(fileName);
  vector<Triangle> triangleList = this->triangles;
  sort(triangleList.begin(), triangleList.end(), [](const Triangle& tri1, const Triangle& tri2)
       {
        float xPoint1 = (tri1.points[0].x + tri1.points[1].x + tri1.points[2].x) / 3.0f;
        float xPoint2 = (tri2.points[0].x + tri2.points[1].x + tri2.points[2].x) / 3.0f;

        float yPoint1 = (tri1.points[0].y + tri1.points[1].y + tri1.points[2].y) / 3.0f;
        float yPoint2 = (tri2.points[0].y + tri2.points[1].y + tri2.points[2].y) / 3.0f;

        float zPoint1 = (tri1.points[0].z + tri1.points[1].z + tri1.points[2].z) / 3.0f;
        float zPoint2 = (tri2.points[0].z + tri2.points[1].z + tri2.points[2].z) / 3.0f;
          
        if(xPoint1 != xPoint2) return xPoint1 < xPoint2;
        if(yPoint1 != yPoint2) return yPoint1 < yPoint2;
        return zPoint1 < zPoint2;
       });

  for(const auto& triangle : triangleList)
  {
    outputFile << triangle.ExtractInfo() << '\n';
  }
  outputFile.close();
}

Mesh* Mesh::Duplicate()
{
    Mesh* result = new Mesh();
    BULK_COPY_ARRAY(this->translationOffsets, result->translationOffsets, 3);
    BULK_COPY_ARRAY(this->rotationDegrees, result->rotationDegrees, 3);
    BULK_COPY_ARRAY(this->rotationSpeeds, result->rotationSpeeds, 3);
    BULK_COPY_ARRAY(this->scalingOffsets, result->scalingOffsets, 3);
    BULK_COPY_ARRAY(this->doAutomaticRotations, result->doAutomaticRotations, 3);

    result->doAutomaticRotation = this->doAutomaticRotation;
    result->triangles = this->triangles;
    result->isStatic = this->isStatic;

    result->forwardVector = this->forwardVector;
    result->lookAtVector = this->lookAtVector;

    result->textureImagePath = this->textureImagePath;
    result->materialType = this->materialType;

    if(this->diffuseColor != nullptr)
    {
      result->SetDiffuseColor(this->diffuseColor->r, this->diffuseColor->g, 
                              this->diffuseColor->g, this->diffuseColor->a);
    }

    if(this->textureImageSprite != nullptr)
    {
        result->textureImageSprite = this->textureImageSprite->Duplicate();
        result->textureImageDecal = new olc::Decal(result->textureImageSprite);
    }
    
    if(this->textureImagePath != nullptr) 
    {
      result->textureImagePath = new string(*(this->textureImagePath));
    }
    
    return result;
}

//Note: Refactor later so the code isn't duplicated
//Its 4AM and I am too tired to do this now
void PrintTrianglesToDisk(const vector<Triangle>& input, const string& fileName)
{
  ofstream outputFile(fileName);
  vector<Triangle> triangleList = input;
  outputFile << "Total Triangles: " << input.size() << '\n';
  sort(triangleList.begin(), triangleList.end(), [](const Triangle& tri1, const Triangle& tri2)
       {
        float xPoint1 = (tri1.points[0].x + tri1.points[1].x + tri1.points[2].x) / 3.0f;
        float xPoint2 = (tri2.points[0].x + tri2.points[1].x + tri2.points[2].x) / 3.0f;

        float yPoint1 = (tri1.points[0].y + tri1.points[1].y + tri1.points[2].y) / 3.0f;
        float yPoint2 = (tri2.points[0].y + tri2.points[1].y + tri2.points[2].y) / 3.0f;

        float zPoint1 = (tri1.points[0].z + tri1.points[1].z + tri1.points[2].z) / 3.0f;
        float zPoint2 = (tri2.points[0].z + tri2.points[1].z + tri2.points[2].z) / 3.0f;
          
        if(xPoint1 != xPoint2) return xPoint1 < xPoint2;
        if(yPoint1 != yPoint2) return yPoint1 < yPoint2;
        return zPoint1 < zPoint2;
       });
  for(const auto& triangle : triangleList)
  {
    outputFile << triangle.ExtractInfo() << '\n';
  }
  outputFile.close();
}

Mesh::~Mesh()
{
  if(this->diffuseColor != nullptr)
    delete this->diffuseColor;
  if(this->textureImageSprite != nullptr)
    delete this->textureImageSprite;
  if(this->textureImageDecal != nullptr)
    delete this->textureImageDecal;

  if(this->textureImagePath != nullptr)
    delete this->textureImagePath;
}

void Mesh::SetTranslationOffsets(const float& newX, const float& newY, const float& newZ)
{
  this->translationOffsets[0] = newX;
  this->translationOffsets[1] = newY;
  this->translationOffsets[2] = newZ;
}

void Mesh::SetTranslationOffsets(const Vector3D& newTranslations)
{
  SetTranslationOffsets(newTranslations.x, newTranslations.y, newTranslations.z);
}

void Mesh::SetRotationSpeeds(const float& newX, const float& newY, const float& newZ)
{
  this->rotationSpeeds[0] = newX;
  this->rotationSpeeds[1] = newY;
  this->rotationSpeeds[2] = newZ;
}

void Mesh::SetRotationSpeeds(const Vector3D& inputVector)
{
  this->rotationSpeeds[0] = inputVector.x;
  this->rotationSpeeds[1] = inputVector.y;
  this->rotationSpeeds[2] = inputVector.z;
}

void Mesh::SetScalingOffsets(const float& newX, const float& newY, const float& newZ)
{
  this->scalingOffsets[0] = newX;
  this->scalingOffsets[1] = newY;
  this->scalingOffsets[2] = newZ;
}

void Mesh::SetDiffuseColor(const uint8_t& rVal, const uint8_t& gVal, const uint8_t& bVal, const uint8_t& aVal)
{
  if(this->diffuseColor != nullptr) delete this->diffuseColor;
  this->diffuseColor = new olc::Pixel(rVal, gVal, bVal, aVal);
  this->materialType = MATERIAL_TYPES::DIFFUSE;
}

void Mesh::SetDiffuseColor(const olc::Pixel& inputPixel)
{
  if(this->diffuseColor != nullptr) 
    delete this->diffuseColor;
  this->diffuseColor = new olc::Pixel(inputPixel);
  this->materialType = MATERIAL_TYPES::DIFFUSE;
}

bool Mesh::SetTextureImage(const string& pathToImage)
{
  this->materialType = MATERIAL_TYPES::TEXTURE;
  if(this->diffuseColor != nullptr)
    delete this->diffuseColor;
  
  if(this->textureImageSprite != nullptr)
    delete this->textureImageSprite;
  if(this->textureImageDecal != nullptr)
    delete this->textureImageDecal;

  if(filesystem::exists(pathToImage))
  {
    this->textureImageSprite = new olc::Sprite(pathToImage);
    this->textureImageDecal = new olc::Decal(this->textureImageSprite);
    this->textureImagePath = new string(pathToImage);
    return true;
  }

  else
  {
    string missingTexturePath = GetPathFromResources({"textures", "missingTexture.png"});
    this->textureImageSprite = new olc::Sprite(missingTexturePath);
    this->textureImageDecal = new olc::Decal(this->textureImageSprite);
    this->textureImagePath = &missingTexturePath;
    return false;
  }
}

bool Mesh::SetMeshName(const string& newName)
{
  if(newName.empty())
    return false;
  this->meshName = newName;
  return true;
}

bool Mesh::LoadFromOBJFile(const string& fileName, bool hasTexture)
{
  //Meshes are immutable once created
  if(this->triangles.size() != 0)
    return false;

  this->hasTexture = hasTexture;
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
    char lineBuffer[512];
    char junkChar;
    //cout << "Currently on line: " << lineCounter << '\n';
    inputFile.getline(lineBuffer, 512);
    strstream stringStream;
    stringStream << lineBuffer;

    if(lineBuffer[0] == 'v')
    {
      if(lineBuffer[1] == 't')
      {
        Vector2D tempTextel;
        stringStream >> junkChar >> junkChar >> tempTextel.u >> tempTextel.v;
        tempTextel.v = 1.0f - tempTextel.v;
        textels.push_back(tempTextel);
      }
      else if(lineBuffer[1] == 'n')
      {
        // Ignore normals for now
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

				string tokens[20];
				int nTokenCount = -1;


				while (!stringStream.eof())
				{
					char c = stringStream.get();
					if (c == ' ' || c == '/')
						nTokenCount++;
					else if (nTokenCount >= 0 && nTokenCount < 20)
						tokens[nTokenCount].append(1, c);
				}

				// tokens[nTokenCount].pop_back(); // Removed potentially unsafe pop_back

        if (nTokenCount >= 8) // v/vt/vn or v//vn
        {
          int v1 = stoi(tokens[0]) - 1;
          int v2 = stoi(tokens[3]) - 1;
          int v3 = stoi(tokens[6]) - 1;
          
          // Handle texture coordinates if present
          Vector2D t1 = (!tokens[1].empty()) ? textels[stoi(tokens[1]) - 1] : Vector2D();
          Vector2D t2 = (!tokens[4].empty()) ? textels[stoi(tokens[4]) - 1] : Vector2D();
          Vector2D t3 = (!tokens[7].empty()) ? textels[stoi(tokens[7]) - 1] : Vector2D();

				  triangles.push_back({ vertices[v1], vertices[v2], vertices[v3], t1, t2, t3 });

          if (nTokenCount >= 11) // Quad: Add second triangle (v1, v3, v4)
          {
             int v4 = stoi(tokens[9]) - 1;
             Vector2D t4 = (!tokens[10].empty()) ? textels[stoi(tokens[10]) - 1] : Vector2D();
             triangles.push_back({ vertices[v1], vertices[v3], vertices[v4], t1, t3, t4 });
          }
        }
        else if (nTokenCount >= 5) // v/vt
        {
          int v1 = stoi(tokens[0]) - 1;
          int v2 = stoi(tokens[2]) - 1;
          int v3 = stoi(tokens[4]) - 1;
          
          Vector2D t1 = textels[stoi(tokens[1]) - 1];
          Vector2D t2 = textels[stoi(tokens[3]) - 1];
          Vector2D t3 = textels[stoi(tokens[5]) - 1];

				  triangles.push_back({ vertices[v1], vertices[v2], vertices[v3], t1, t2, t3 });

          if (nTokenCount >= 7) // Quad: Add second triangle (v1, v3, v4)
          {
             int v4 = stoi(tokens[6]) - 1;
             Vector2D t4 = textels[stoi(tokens[7]) - 1];
             triangles.push_back({ vertices[v1], vertices[v3], vertices[v4], t1, t3, t4 });
          }
        }
      }
    }
    lineCounter++;
  }
  this->triangles = triangles;
  this->objFilePath = fileName;
  return true;
}

Mesh::Mesh()
{
  this->materialType = MATERIAL_TYPES::NONE;
}

Mesh::Mesh(const std::string& name)
{
  this->materialType = MATERIAL_TYPES::NONE;
  this->meshName = name;
}

const int MeshList::GetTotalTriangles() const
{
  return this->totalTriangles;
}

const int MeshList::GetTotalVertices() const
{
  return this->totalVertices;
}

const string& Mesh::GetObjectFilePath() const
{
  return this->objFilePath;
}

deque<Mesh*>& MeshList::GetMeshList()
{
  return this->meshList;
}

void MeshList::UpdateTotalCounts()
{
  int newTotalTriangles = 0;
  for(int i = 0; i < this->meshList.size(); i++)
    newTotalTriangles += meshList[i]->GetTotalTriangles();
  this->totalTriangles = newTotalTriangles;
  this->totalVertices = newTotalTriangles * 3;
}

//Consideration: performance of std::move
void MeshList::AppendMesh(Mesh* input)
{
  this->meshList.push_back(input);
}

bool MeshList::LoadMeshFromOBJ(string fileName)
{
  Mesh* temp = new Mesh();
  if(temp->LoadFromOBJFile(fileName) == false)
    return false;
  this->meshList.push_back(temp);
  return true;
}
