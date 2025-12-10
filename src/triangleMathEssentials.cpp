#include "triangleMathEssentials.h"
#include "matrixMathEssentials.h"
#include "vectorMathEssentials.h"
#include "timerUtility.h"

Triangle MultiplyTriangle(const Triangle &input, const Matrix4x4& transformMatrix, bool normalize)
{
  Triangle toReturn;
  toReturn.color = input.color;
  for(int i = 0; i < 3; i++)
    toReturn.texels[i] = input.texels[i];
  MultiplyMatrixVector(input.points[0], transformMatrix, toReturn.points[0], normalize);
  MultiplyMatrixVector(input.points[1], transformMatrix, toReturn.points[1], normalize);
  MultiplyMatrixVector(input.points[2], transformMatrix, toReturn.points[2], normalize);
  return toReturn;
}

Triangle ScaleTriangle(const Triangle& input, float xScale, float yScale, float zScale)
{
  Triangle toReturn = input;
  toReturn.points[0].x *= xScale; toReturn.points[0].y *= yScale; toReturn.points[0].z *= zScale;
  toReturn.points[1].x *= xScale; toReturn.points[1].y *= yScale; toReturn.points[1].z *= zScale;
  toReturn.points[2].x *= xScale; toReturn.points[2].y *= yScale; toReturn.points[2].z *= zScale;
  return toReturn;
}

Triangle RotateTriangle(const Triangle& input, const Matrix4x4& transformMatrix)
{
  return MultiplyTriangle(input, transformMatrix);
}

Triangle TranslateTriangle(const Triangle& input, float moveX, float moveY, float moveZ)
{
  Triangle toReturn = input;
  toReturn.points[0].x += moveX; toReturn.points[1].x += moveX; toReturn.points[2].x += moveX;
  toReturn.points[0].y += moveY; toReturn.points[1].y += moveY; toReturn.points[2].y += moveY;
  toReturn.points[0].z += moveZ; toReturn.points[1].z += moveZ; toReturn.points[2].z += moveZ;
  return toReturn;
}

void TranslateTriangleIP(Triangle& input, float moveX, float moveY, float moveZ)
{
  for(int i = 0; i < 3; i++)
  {
    input.points[i].x += moveX;
    input.points[i].y += moveY;
    input.points[i].z += moveZ;
  }
}

void ScreenNormalizeTriangle(Triangle& input, const float HalfScreenWidth, const float HalfScreenHeight)
{
  input.points[0].x += 1.0f; input.points[1].x += 1.0f; input.points[2].x += 1.0f;
  input.points[0].y += 1.0f; input.points[1].y += 1.0f; input.points[2].y += 1.0f;
  input.points[0].x *= HalfScreenWidth;
  input.points[0].y *= HalfScreenHeight;
  input.points[1].x *= HalfScreenWidth;
  input.points[1].y *= HalfScreenHeight;
  input.points[2].x *= HalfScreenWidth;
  input.points[2].y *= HalfScreenHeight;
}

//Currently, it only inverts the Y 
//I will fix the projection matrix later
void InvertTriangleXY(Triangle& input)
{
    /*input.points[0].x *= -1.0f;
	input.points[1].x *= -1.0f;
	input.points[2].x *= -1.0f;*/
  //ScopedTimer inversionTime("Y INVERSION TIME");
	/*input.points[0].y *= -1.0f;
	input.points[1].y *= -1.0f;
	input.points[2].y *= -1.0f;*/
  //input.points[0].y ^=  2147483648;
  //This micro optimization most likely has absolutely no effect
  input.points[0].y = std::bit_cast<float>(
    std::bit_cast<uint32_t>(input.points[0].y) ^ 0x80000000u
  );

  input.points[1].y = std::bit_cast<float>(
    std::bit_cast<uint32_t>(input.points[1].y) ^ 0x80000000u
);

  input.points[2].y = std::bit_cast<float>(
    std::bit_cast<uint32_t>(input.points[2].y) ^ 0x80000000u
);
}

void InvertTriangleX(Triangle& input)
{
    input.points[0].x *= -1.0f;
    input.points[1].x *= -1.0f;
    input.points[2].x *= -1.0f;
}

void InvertTriangleY(Triangle& input)
{
    input.points[0].y *= -1.0f;
    input.points[1].y *= -1.0f;
    input.points[2].y *= -1.0f;
}

void NormalizeTriangleTextels(Triangle& input)
{
  DivideVectorScalar(input.texels[0], input.points[0].w);
  DivideVectorScalar(input.texels[1], input.points[1].w);
  DivideVectorScalar(input.texels[2], input.points[2].w);
  
  //This is so that we have a relation between view space and between projection sapce
  input.texels[0].w = 1.0f / input.points[0].w;
  input.texels[1].w = 1.0f / input.points[1].w;
  input.texels[2].w = 1.0f / input.points[2].w;
}

Vector3D GetMidpoint(const Triangle& input)
{
  Vector3D toReturn;
  toReturn.x = (input.points[0].x + input.points[1].x + input.points[2].x) / 3.0f;
  toReturn.y = (input.points[0].y + input.points[1].y + input.points[2].y) / 3.0f;
  toReturn.z = (input.points[0].z + input.points[1].z + input.points[2].z) / 3.0f;
  return toReturn;
}

void ConvertToDNCoordinates(Triangle& input)
{
  DivideVectorScalar(input.points[0], input.points[0].w);
  DivideVectorScalar(input.points[1], input.points[1].w);
  DivideVectorScalar(input.points[2], input.points[2].w);
  //I don't know why this is here
  //Removing it made things work though!
  //for(int i = 0; i < 3; i++)
    //input.points[i].w = 1.0f;
}

