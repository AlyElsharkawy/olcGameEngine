#define OLC_PGE_APPLICATION
#define OLC_IMAGE_STB
#include <iostream>
#include "matrixMathEssentials.h"
#include "geometricPrimitives.h"
#include "vectorMathEssentials.h"

int main()
{
  while(true)
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
  }
  return 0;
}
