#include "meshMathEssentials.h"
#include "vectorMathEssentials.h"

Vector3D GetCentroid(const Mesh& input)
{
  Vector3D output;
  int vertexCount = 0;
  for(const auto& triangle : input.triangles)
  {
    for(const auto& vertex : triangle.points)
    {
      output.x += vertex.x;
      output.y += vertex.y;
      output.z += vertex.z;
      ++vertexCount;
    }
  }
  DivideVectorScalar(output, vertexCount);
  return output;
}

