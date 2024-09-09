#include "vectorMathEssentials.h"

void NormalizeVector(Vector3D& input)
{
  float vectorLength = GetVectorLength(input);
  if(vectorLength == 0.0f)
    return;
  DivideVectorScalar(input, vectorLength);
}

Vector3D AddVector(const Vector3D& input1, const Vector3D& input2)
{
  Vector3D toReturn;
  toReturn.x = input1.x + input2.x;
  toReturn.y = input1.y + input2.y;
  toReturn.z = input1.z + input2.z;
  return toReturn;
}

void AddVectorIP(Vector3D& input1, const Vector3D& input2)
{
  input1.x += input2.x;
  input1.y += input2.y;
  input1.z += input2.z;
}


Vector2D AddVector(const Vector2D& input1, const Vector2D& input2)
{
  Vector2D toReturn;
  toReturn.u = input1.u + input2.u;
  toReturn.v = input1.v + input2.v;
  return toReturn;
}

Vector3D SubtractVector(const Vector3D& input1, const Vector3D& input2)
{
  Vector3D toReturn;
  toReturn.x = input1.x - input2.x;
  toReturn.y = input1.y - input2.y;
  toReturn.z = input1.z - input2.z;
  return toReturn;
}

void SubtractVectorIP(Vector3D& input1, const Vector3D& input2)
{
  input1.x -= input2.x;
  input1.y -= input2.y;
  input1.z -= input2.z;
}

Vector2D SubtractVector(const Vector2D& input1, const Vector2D& input2)
{
  Vector2D toReturn;
  toReturn.u = input1.u - input2.v;
  toReturn.v = input2.v - input1.v;
  return toReturn;
}

Vector3D GetCrossProduct(const Vector3D &line1, const Vector3D &line2)
{
  Vector3D toReturn;
  toReturn.x = line1.y * line2.z - line1.z * line2.y;
  toReturn.y = line1.z * line2.x - line1.x * line2.z;
  toReturn.z = line1.x * line2.y - line1.y * line2.x;
  return toReturn;
}

float GetDotProduct(const Vector3D& vectorA, const Vector3D& vectorB, const Vector3D& offset)
{
  return vectorA.x * (vectorB.x - offset.x) + vectorA.y * (vectorB.y - offset.y) + vectorA.z * (vectorB.z - offset.z);
}

void MultiplyVectorScalar(Vector3D& input, float scalarValue)
{
 input.x *= scalarValue; input.y *= scalarValue; input.z *= scalarValue; 
}

Vector3D MultiplyVectorScalarOP(const Vector3D& input, float scalarValue)
{
  Vector3D toReturn = input;
  toReturn.x *= scalarValue;
  toReturn.y *= scalarValue;
  toReturn.z *= scalarValue;
  return toReturn;
}

void MultiplyVectorScalar(Vector2D& input, float scalarValue)
{
  input.u *= scalarValue; input.v *= scalarValue;
}

void DivideVectorScalar(Vector3D& input, float scalarValue)
{
  input.x /= scalarValue; input.y /= scalarValue; input.z /= scalarValue;
}

void DivideVectorScalar(Vector2D& input, float scalarValue)
{
  input.u /= scalarValue; input.v /= scalarValue;
}

float GetVectorLength(const Vector3D &input)
{
  return sqrtf(input.x * input.x + input.y * input.y + input.z * input.z);
}

float GetDistanceBetweenPoints(const Vector3D& input1, const Vector3D& input2)
{
 return sqrtf(pow(input2.x - input1.x, 2) + pow(input2.y - input1.y,2) + pow(input2.z - input1.z,2)); 
}

Vector3D GetCentroid(const Mesh& input)
{
  Vector3D output;
  for(const auto& triangle : input.triangles)
  {
    for(const auto& vertex : triangle.points)
    {
      output.x += vertex.x;
      output.y += vertex.y;
      output.z += vertex.z;
    }
  }
  DivideVectorScalar(output, input.triangles.size());
}

bool IsZeroVector(const Vector3D& input)
{
  return input.x == 0.0f && input.y == 0.0f && input.z == 0.0f;
}

