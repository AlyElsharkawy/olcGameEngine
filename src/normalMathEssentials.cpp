#include "geometricPrimitives.h"
#include "normalMathEssentials.h"
#include "vectorMathEssentials.h"
#include <complex>

Vector3D GetNormal(const Triangle& input, bool normalize)
{
  Vector3D line1, line2, normalOutput;

  //Extract lines 1 and 2 from triangle
  line1.x = input.points[1].x - input.points[0].x;
  line1.y = input.points[1].y - input.points[0].y;
  line1.z = input.points[1].z - input.points[0].z;
    
  line2.x = input.points[2].x - input.points[0].x;
  line2.y = input.points[2].y - input.points[0].y;
  line2.z = input.points[2].z - input.points[0].z;

  //Compute Cross Product
  normalOutput.x = line1.y * line2.z - line1.z * line2.y;
  normalOutput.y = line1.z * line2.x - line1.x * line2.z;
  normalOutput.z = line1.x * line2.y - line1.y * line2.x;
  
  //Normalize the normal which is a normal thing to do
  if(normalize == true)
    NormalizeVector(normalOutput); 
  //Return result
  return normalOutput;
}

void ScreenNormalizeNormal(Vector3D& input, float ScreenWidth, float ScreenHeight)
{
  input.x += 1.0f; input.y += 1.0f;
  input.x *= 0.5f * ScreenWidth;
  input.y *= 0.5f * ScreenHeight;
}

Vector3D ScaleNormal(const Vector3D& source, const Vector3D& normal, const float& length)
{
  Vector3D temp = normal;
  MultiplyVectorScalar(temp, length);
  AddVectorIP(temp, source);
  return temp;
}
