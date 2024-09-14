#define OLC_PGE_APPLICATION
#define OLC_IMAGE_STB
#include <iostream>
#include "matrixMathEssentials.h"
#include "geometricPrimitives.h"
#include "vectorMathEssentials.h"
#include "normalMathEssentials.h"

int main()
{
  /*while(true)
  {
    Vector3D sourceVector;
    Vector3D targetVector;
    Vector3D output;
    float temp1, temp2, temp3;
    cout << "Enter source vector values: ";
    cin >> temp1 >> temp2 >> temp3; cout << '\n';
    sourceVector = {temp1, temp2, temp3};
    cout << "Enter target vector values: ";
    cin >> temp1 >> temp2 >> temp3; cout << '\n';
    targetVector = {temp1,temp2,temp3};
    NormalizeVector(targetVector);
    Matrix4x4 rotationMatrix = GetLookAtRotationMatrix(sourceVector, targetVector);
    MultiplyMatrixVector(sourceVector, rotationMatrix, output);
    cout << "Source Vector: ";
    sourceVector.PrintPoint();
    cout << "Target Vector: ";
    targetVector.PrintPoint();
    cout << "New source vector: ";
    output.PrintPoint();
    cout << "\n\n";
  }*/ 
  /*const float FIELD_OF_VIEW = 90.0f;
  const float mathPI = 3.14159f;
  const float FIELD_OF_VIEW_RADIANS = 1.0f / tanf(FIELD_OF_VIEW * 0.5 / 180.0f * mathPI);
  Matrix4x4 proj = GetProjectionMatrix(1080.0f / 1920.0f, FIELD_OF_VIEW_RADIANS, 0.1f, 1000.0f);
  proj.PrintMatrix();*/ 
  Vector3D source(10,9,0);
  Vector3D destination(50,60,0);
  Vector3D newVec = ScaleNormal(source, destination, 8.0f);
  newVec.PrintPoint();
  return 0;
}
