#pragma once
#include "geometricPrimitives.h"

Matrix4x4 GetProjectionMatrix(float aspectRatio, float fieldOfViewModified, float visionNear, float visionFar);
void UpdateProjectionMatrix(Matrix4x4& inputMatrix, float newAspectRatio,float newFieldOfViewModifed, float newVisionNear, float newVisionFar);
Matrix4x4 GetRotationMatrix(short ROT_TYPE, float fRadians);
Matrix4x4 GetCompoundRotationMatrix(short ROT_TYPE, float fRadiansX, float fRadiansY, float fRadiansZ);
Matrix4x4 GetInverseRotationMatrix(short ROT_TYPE, float fRadians);
Matrix4x4 GetArbitraryRotationMatrix(const Vector3D& axis, float fRadians);
Matrix4x4 GetTranslationMatrix(float xTrans, float yTrans, float zTrans);
Matrix4x4 GetPointAtMatrix(const Vector3D& position, const Vector3D& target, const Vector3D& up);
void UpdateTranslationmatrix(Matrix4x4& inputMatrix, float xTrans, float yTrans, float zTrans);
void UpdateRotationMatrix(Matrix4x4& inputMatrix, short ROT_TYPE, float fNewRadians);
Matrix4x4 GetIdentityMatrix();
void MultiplyMatrixVector(const Vector3D& vectorInput, const Matrix4x4& inputMatrix, Vector3D& outputVector, bool normalize = false);
void MultiplyMatrixScalar(Matrix4x4& inputMatrix, float scalarValue);
Matrix4x4 AddMatrix(const Matrix4x4& matrix1, const Matrix4x4& matrix2);
void AddMatrix(Matrix4x4& matrix1, const Matrix4x4& matrix2);
Matrix4x4 SubtractMatrix(const Matrix4x4& matrix1, const Matrix4x4& matrix2);
void SubtractMatrix(Matrix4x4& matrix1, const Matrix4x4& matrix2);
Matrix4x4 MultiplyMatrixMatrix(const Matrix4x4& matrix1, const Matrix4x4& matrix2);
Matrix4x4 InvertPointAtMatrix(const Matrix4x4& m);
void DecomposeRotationMatrix(const Matrix4x4& inputMatrix, float& rotationX, float& rotationY, float& rotationZ);
Matrix4x4 GetLookAtRotationMatrix(const Vector3D& sourceVector, const Vector3D& lookAtVector);
