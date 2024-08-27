#include <iostream>
#include <cmath>
#include "matrixMathEssentials.h"
#include "geometricPrimitives.h"
#include "vectorMathEssentials.h"
#include "globalVariables.h"

Matrix4x4 GetProjectionMatrix(float aspectRatio, float fieldOfViewModified, float visionNear, float visionFar)
{
  Matrix4x4 toReturn;
  toReturn.mat[0][0] = aspectRatio * fieldOfViewModified;
  toReturn.mat[1][1] = fieldOfViewModified;
  toReturn.mat[2][2] = visionFar / (visionFar - visionNear);
  toReturn.mat[3][2] = (-visionFar * visionNear) / (visionFar - visionNear);
  toReturn.mat[2][3] = 1.0f;
  toReturn.mat[3][3] = 0.0f;
  return toReturn;
}

void UpdateProjectionMatrix(Matrix4x4& inputMatrix, float newAspectRatio,float newFieldOfViewModifed, float newVisionNear, float newVisionFar)
{
  inputMatrix.mat[0][0] = newAspectRatio * newFieldOfViewModifed;
  inputMatrix.mat[1][1] = newFieldOfViewModifed;
  inputMatrix.mat[2][2] = newVisionFar / (newVisionFar - newVisionNear);
  inputMatrix.mat[3][2] = (-newVisionFar * newVisionNear) / (newVisionFar - newVisionNear);
  inputMatrix.mat[2][3] = 1.0f;
}

Matrix4x4 GetRotationMatrix(short ROT_TYPE, float fRadians)
{
  Matrix4x4 toReturn = GetIdentityMatrix();
  switch(ROT_TYPE)
  {
    case  ROT_TYPES::ROT_X:
      {
        toReturn.mat[1][1] = cosf(fRadians);
        toReturn.mat[2][2] = cosf(fRadians);
        toReturn.mat[2][1] = sinf(fRadians);
        toReturn.mat[1][2] = -sinf(fRadians);
        break;
      }
    case ROT_TYPES::ROT_Y:
      {
        toReturn.mat[0][0] = cosf(fRadians);
        toReturn.mat[0][2] = sinf(fRadians);
        toReturn.mat[2][0] = -sinf(fRadians);
        toReturn.mat[2][2] = cosf(fRadians);
        break;
      }
    case ROT_TYPES::ROT_Z:
      {
        toReturn.mat[0][0] = cosf(fRadians);
        toReturn.mat[0][1] = -sinf(fRadians);
        toReturn.mat[1][0] = sinf(fRadians);
        toReturn.mat[1][1] = cosf(fRadians);
        break;
      }
    default:
      cerr << "FATAL ERROR: Unkown matrix initialized. Please report to developer!\n";
  }
  return toReturn;
}

Matrix4x4 GetCompoundRotationMatrix(short ROT_TYPE, float fRadiansX, float fRadiansY, float fRadiansZ)
{
  switch(ROT_TYPE)
  {
    case ROT_TYPES::ROT_ZYX:
      {
        Matrix4x4 xRotationMatrix = GetRotationMatrix(ROT_TYPES::ROT_X, fRadiansX);
        Matrix4x4 yRotationMatrix = GetRotationMatrix(ROT_TYPES::ROT_Y, fRadiansY);
        Matrix4x4 zRotationMatrix = GetRotationMatrix(ROT_TYPES::ROT_Z, fRadiansZ);
        Matrix4x4 temp1 = MultiplyMatrixMatrix(zRotationMatrix, yRotationMatrix);
        Matrix4x4 temp2 = MultiplyMatrixMatrix(temp1, xRotationMatrix);
        return temp2;
        break;
      }
    default:
      cerr << "FATAL ERROR: Unkown matrix initialized. Please report to developer";
    //STFU clangd!
    return GetIdentityMatrix();
  }
}

Matrix4x4 GetInverseRotationMatrix(short ROT_TYPE, float fRadians)
{
  Matrix4x4 toReturn = GetIdentityMatrix();
  switch(ROT_TYPE)
  {
    case ROT_TYPES::ROT_X:
      {
        toReturn.mat[1][1] = cosf(fRadians);
        toReturn.mat[1][2] = sinf(fRadians);
        toReturn.mat[2][1] = -sinf(fRadians);
        toReturn.mat[2][2] = cosf(fRadians);
        break;
      }
    case ROT_TYPES::ROT_Y:
      {
        toReturn.mat[0][0] = cosf(fRadians);
        toReturn.mat[0][2] = -sinf(fRadians);
        toReturn.mat[2][0] = sinf(fRadians);
        toReturn.mat[2][2] = cosf(fRadians);

        break;
      }
    case ROT_TYPES::ROT_Z:
      {
        toReturn.mat[0][0] = cosf(fRadians);
        toReturn.mat[0][1] = sinf(fRadians);
        toReturn.mat[1][0] = -sinf(fRadians);
        toReturn.mat[1][1] = cosf(fRadians);
        break;
      }
    default:
      cerr << "FATAL ERROR: Unkown matrix type initialized. Please report to developer";
  }
  return toReturn;
}

Matrix4x4 GetArbitraryRotationMatrix(const Vector3D& axis, float fRadians)
{
  //Vector Point = (A,B,C)
  //const float axisLength = GetVectorLength(axis);
  const float vVal = sqrtf(axis.y * axis.y + axis.z * axis.z);
  const float toRadians = mathPI / 180.0f;
  const float xTheta = asinf((axis.z / vVal)) * toRadians;
  const float yTheta = asinf((axis.x / GetVectorLength(axis))) * toRadians;
  
  Matrix4x4 xRotationMatrix;
  Matrix4x4 xRotationMatrixInverse;
  if(vVal == 0.0f)
  {
    xRotationMatrix = GetIdentityMatrix();
    xRotationMatrixInverse = GetIdentityMatrix();
  }
  else
  {
    xRotationMatrix = GetRotationMatrix(ROT_TYPES::ROT_X, xTheta);
    xRotationMatrixInverse = GetInverseRotationMatrix(ROT_TYPES::ROT_X, xTheta);
  }
  Matrix4x4 yRotationMatrix = GetRotationMatrix(ROT_TYPES::ROT_Y, yTheta);
  Matrix4x4 zRotationMatrix = GetRotationMatrix(ROT_TYPES::ROT_Z, fRadians);
  Matrix4x4 yRotationMatrixInverse = GetInverseRotationMatrix(ROT_TYPES::ROT_Y, yTheta);
  
  Matrix4x4 temp1 = MultiplyMatrixMatrix(xRotationMatrix, yRotationMatrix);
  Matrix4x4 temp2 = MultiplyMatrixMatrix(temp1, zRotationMatrix);
  Matrix4x4 temp3 = MultiplyMatrixMatrix(temp2, yRotationMatrixInverse);
  Matrix4x4 temp4 = MultiplyMatrixMatrix(temp3, xRotationMatrixInverse);
  return temp4;
}

void UpdateRotationMatrix(Matrix4x4& inputMatrix, short ROT_TYPE, float fNewRadians)
{
  switch(ROT_TYPE)
  {
    case ROT_TYPES::ROT_X:
      {
        inputMatrix.mat[1][1] = cosf(fNewRadians);
        inputMatrix.mat[2][2] = cosf(fNewRadians);
        inputMatrix.mat[2][1] = sinf(fNewRadians);
        inputMatrix.mat[1][2] = -sinf(fNewRadians);
        break;
      }
    case ROT_TYPES::ROT_Y:
      {
        inputMatrix.mat[0][0] = cosf(fNewRadians);
        inputMatrix.mat[0][2] = sinf(fNewRadians);
        inputMatrix.mat[2][0] = -sinf(fNewRadians);
        inputMatrix.mat[2][2] = cosf(fNewRadians);
        break;
      }
    case ROT_TYPES::ROT_Z:
      {
        inputMatrix.mat[0][0] = cosf(fNewRadians);
        inputMatrix.mat[0][1] = -sinf(fNewRadians);
        inputMatrix.mat[1][0] = sinf(fNewRadians);
        inputMatrix.mat[1][1] = cosf(fNewRadians);
        break;
      }
    default:
      cerr << "FATAL ERROR: Unkown matrix modified. Please report to developer!\n";
  }
}

Matrix4x4 GetIdentityMatrix()
{
  Matrix4x4 toReturn;
  toReturn.mat[0][0] = 1.0f;
  toReturn.mat[1][1] = 1.0f;
  toReturn.mat[2][2] = 1.0f;
  toReturn.mat[3][3] = 1.0f;
  return toReturn;
}

void MultiplyMatrixVector(const Vector3D& vectorInput, const Matrix4x4& inputMatrix, Vector3D& outputVector, bool normalize)
{
  /*
  //Old implemenation. Kept for legacy support. ANd cause new implemenation won't work!
  outputVector.x = vectorInput.x * inputMatrix.mat[0][0] + vectorInput.y * inputMatrix.mat[1][0] + vectorInput.z * inputMatrix.mat[2][0] + inputMatrix.mat[3][0];

  outputVector.y = vectorInput.x * inputMatrix.mat[0][1] + vectorInput.y * inputMatrix.mat[1][1] + vectorInput.z * inputMatrix.mat[2][1] + inputMatrix.mat[3][1];

  outputVector.z = vectorInput.x * inputMatrix.mat[0][2] + vectorInput.y * inputMatrix.mat[1][2] + vectorInput.z * inputMatrix.mat[2][2] + inputMatrix.mat[3][2];

  float zVal = vectorInput.x * inputMatrix.mat[0][3] + vectorInput.y * inputMatrix.mat[1][3] + vectorInput.z * inputMatrix.mat[2][3] + inputMatrix.mat[3][3];

  //If zVal is 0, then we will have infinitely large triangles...not good
  //cout << "Z val is: " << zVal << "\n";
  if(zVal != 0.0f)
  {
    outputVector.x /= zVal;
    outputVector.y /= zVal;
    outputVector.z /= zVal;
  }
  */

  outputVector.x = vectorInput.x * inputMatrix.mat[0][0] + vectorInput.y * inputMatrix.mat[1][0] + vectorInput.z * inputMatrix.mat[2][0] + vectorInput.w * inputMatrix.mat[3][0];
  outputVector.y = vectorInput.x * inputMatrix.mat[0][1] + vectorInput.y * inputMatrix.mat[1][1] + vectorInput.z * inputMatrix.mat[2][1] + vectorInput.w * inputMatrix.mat[3][1];
  outputVector.z = vectorInput.x * inputMatrix.mat[0][2] + vectorInput.y * inputMatrix.mat[1][2] + vectorInput.z * inputMatrix.mat[2][2] + vectorInput.w * inputMatrix.mat[3][2];
  outputVector.w = vectorInput.x * inputMatrix.mat[0][3] + vectorInput.y * inputMatrix.mat[1][3] + vectorInput.z * inputMatrix.mat[2][3] + vectorInput.w * inputMatrix.mat[3][3];

  if(normalize == true)
    DivideVectorScalar(outputVector, outputVector.w);
}

void MultiplyMatrixScalar(Matrix4x4& inputMatrix, float scalarValue)
{
  for(int i = 0; i < 4; i++)
  {
    for(int j = 0; j < 4; j++)
    {
      inputMatrix.mat[i][j] *= scalarValue;
    }
  }
}

Matrix4x4 AddMatrix(const Matrix4x4& matrix1, const Matrix4x4& matrix2)
{
  Matrix4x4 toReturn;
  for(int i = 0; i < 4; i++)
  {
    for(int j = 0; j < 4; j++)
    {
      toReturn.mat[i][j] = matrix1.mat[i][j] + matrix2.mat[i][j];
    }
  }
  return toReturn;
}

void AddMatrix(Matrix4x4& matrix1, const Matrix4x4& matrix2)
{
  for(int i = 0; i < 4; i++)
  {
    for(int j = 0; j < 4; j++)
    {
      matrix1.mat[i][j] += matrix2.mat[i][j];
    }
  }
}

Matrix4x4 SubtractMatrix(const Matrix4x4& matrix1, const Matrix4x4& matrix2)
{
  Matrix4x4 toReturn;
  for(int i = 0; i < 4; i++)
  {
    for(int j = 0; j < 4; j++)
    {
      toReturn.mat[i][j] = matrix1.mat[i][j] - matrix2.mat[i][j];
    }
  }
  return toReturn;
}

void SubtractMatrix(Matrix4x4& matrix1, const Matrix4x4& matrix2)
{
  for(int i = 0; i < 4; i++)
  {
    for(int j = 0; j < 4; j++)
    {
      matrix1.mat[i][j] += matrix2.mat[i][j];
    }
  }
}

Matrix4x4 MultiplyMatrixMatrix(const Matrix4x4& matrix1, const Matrix4x4& matrix2)
{
  Matrix4x4 outputMatrix;
  //Taken from javidx9 video directly
  for(int c = 0; c < 4; c++)
		for(int r = 0; r < 4; r++)
      outputMatrix.mat[r][c] = matrix1.mat[r][0] * matrix2.mat[0][c] + matrix1.mat[r][1] * matrix2.mat[1][c] + matrix1.mat[r][2] * matrix2.mat[2][c] + matrix1.mat[r][3] * matrix2.mat[3][c];
  
  return outputMatrix;


  /*Matrix4x4 outputMatrix;
  for(int i = 0; i < 4; i++)
  {
    for(int j = 0; j < 4; j++)
    {
      for(int k = 0; k < 4; k++)
      {
        outputMatrix.mat[i][j] += matrix1.mat[i][k] * matrix2.mat[k][j];
      }
    }
  }
  return outputMatrix;*/
}

Matrix4x4 GetTranslationMatrix(float xTrans, float yTrans, float zTrans)
{
  Matrix4x4 toReturn;
  toReturn.mat[0][0] = 1.0f;
  toReturn.mat[1][1] = 1.0f;
  toReturn.mat[2][2] = 1.0f;
  toReturn.mat[3][3] = 1.0f;
  toReturn.mat[3][0] = xTrans;
  toReturn.mat[3][1] = yTrans;
  toReturn.mat[3][2] = zTrans;
  return toReturn;
}

void UpdateTranslationmatrix(Matrix4x4& inputMatrix, float xTrans, float yTrans, float zTrans)
{
  inputMatrix.mat[3][0] = xTrans;
  inputMatrix.mat[3][1] = yTrans;
  inputMatrix.mat[3][2] = zTrans;
}

Matrix4x4 GetPointAtMatrix(const Vector3D& position, const Vector3D& target, const Vector3D& up)
{
  //This is the new forward direction 
  Vector3D newForward = SubtractVector(target, position);
  NormalizeVector(newForward);

  //This is the new up direction, float scalarValue)
  Vector3D temp = newForward;
  MultiplyVectorScalar(temp, GetDotProduct(up,newForward));
  Vector3D newUp = SubtractVector(up, temp);
  NormalizeVector(newUp);

  Vector3D newRight = GetCrossProduct(newUp, newForward);

  Matrix4x4 matrix;
  matrix.mat[0][0] = newRight.x;matrix.mat[0][1] = newRight.y;matrix.mat[0][2] = newRight.z;matrix.mat[0][3] = 0.0f;
  matrix.mat[1][0] = newUp.x;matrix.mat[1][1] = newUp.y;matrix.mat[1][2] = newUp.z;matrix.mat[1][3] = 0.0f;
  matrix.mat[2][0] = newForward.x;matrix.mat[2][1] = newForward.y;matrix.mat[2][2] = newForward.z;matrix.mat[2][3] = 0.0f;
  matrix.mat[3][0] = position.x;matrix.mat[3][1] = position.y;matrix.mat[3][2] = position.z;matrix.mat[3][3] = 1.0f;
  return matrix;
}

Matrix4x4 InvertPointAtMatrix(const Matrix4x4& m)
{
  Matrix4x4 matrix;
  matrix.mat[0][0] = m.mat[0][0]; matrix.mat[0][1] = m.mat[1][0]; matrix.mat[0][2] = m.mat[2][0]; matrix.mat[0][3] = 0.0f;
  matrix.mat[1][0] = m.mat[0][1]; matrix.mat[1][1] = m.mat[1][1]; matrix.mat[1][2] = m.mat[2][1]; matrix.mat[1][3] = 0.0f;
  matrix.mat[2][0] = m.mat[0][2]; matrix.mat[2][1] = m.mat[1][2]; matrix.mat[2][2] = m.mat[2][2]; matrix.mat[2][3] = 0.0f;
  matrix.mat[3][0] = -(m.mat[3][0] * matrix.mat[0][0] + m.mat[3][1] * matrix.mat[1][0] + m.mat[3][2] * matrix.mat[2][0]);
  matrix.mat[3][1] = -(m.mat[3][0] * matrix.mat[0][1] + m.mat[3][1] * matrix.mat[1][1] + m.mat[3][2] * matrix.mat[2][1]);
  matrix.mat[3][2] = -(m.mat[3][0] * matrix.mat[0][2] + m.mat[3][1] * matrix.mat[1][2] + m.mat[3][2] * matrix.mat[2][2]);
  matrix.mat[3][3] = 1.0f;
  return matrix;
}

void DecomposeRotationMatrix(const Matrix4x4& inputMatrix, float& rotationX, float& rotationY, float& rotationZ)
{
  //Alpha, beta, gamma 
  rotationY = asinf(-1.0f * inputMatrix.mat[2][0]);
  if(rotationY == 90.0f)
  {
    rotationZ = 0.0f;
    rotationX = atan2f(inputMatrix.mat[0][1], inputMatrix.mat[0][0]);
  }
  if(rotationY == -90.0f)
  {
    rotationZ = 0.0f;
    rotationX = -atan2f(inputMatrix.mat[0][1], inputMatrix.mat[0][0]);
  }
  else
  {
    rotationZ = atan2f(inputMatrix.mat[1][0], inputMatrix.mat[0][0]);
    rotationX = atan2f(inputMatrix.mat[2][1], inputMatrix.mat[2][2]);
  }
}

Matrix4x4 GetLookAtRotationMatrix(const Vector3D& sourceVector ,const Vector3D& lookAtVector)
{
  Matrix4x4 kMatrix;
  Vector3D rotationAxis = GetCrossProduct(sourceVector, lookAtVector);
  
  float sineValue = -GetVectorLength(rotationAxis);
  float cosineValue = GetDotProduct(sourceVector, lookAtVector);

  NormalizeVector(rotationAxis);

  if(sineValue < EPSILON)
  {
    if(cosineValue > 1.0f - EPSILON)
    {
      //This means the vectors are aligned or extremely closely aligned
      //No further rotation calculations are needed
      return GetIdentityMatrix();
    }
    else if(cosineValue < -1.0f + EPSILON)
    {
      //This means the vectors are anti aligned
      //An orthogonal axis will be used for rotation instead 
      Vector3D orthogonalAxis = (abs(sourceVector.x) > EPSILON) ? Vector3D(0,1,0) : Vector3D(1,0,0);
      rotationAxis = GetCrossProduct(orthogonalAxis, sourceVector);
      NormalizeVector(rotationAxis);
    }
    else
    {
      //They aren't neither semi-perfectly aligned or anti-aligned but still close enough
      //to cause issues 
      Vector3D fixedVector = AddVector(lookAtVector, Vector3D(EPSILON,0.0f,0.0));
      rotationAxis = GetCrossProduct(fixedVector, sourceVector);
      sineValue = GetVectorLength(rotationAxis);
      NormalizeVector(rotationAxis);
    }
  }

  kMatrix.mat[0][1] = -1.0f * rotationAxis.z;
  kMatrix.mat[0][2] = rotationAxis.y;
  kMatrix.mat[1][0] = rotationAxis.z;
  kMatrix.mat[1][2] = -1.0f * rotationAxis.x;
  kMatrix.mat[2][0] = -1.0f * rotationAxis.y;
  kMatrix.mat[2][1] = rotationAxis.x;
  
  Matrix4x4 middleTerm = kMatrix;
  MultiplyMatrixScalar(middleTerm, sineValue);
  Matrix4x4 lastTerm = MultiplyMatrixMatrix(kMatrix, kMatrix);
  MultiplyMatrixScalar(lastTerm, (1.0f - cosineValue));

  Matrix4x4 toReturn = GetIdentityMatrix();
  AddMatrix(toReturn, middleTerm);
  AddMatrix(toReturn, lastTerm);
  return toReturn;
}
