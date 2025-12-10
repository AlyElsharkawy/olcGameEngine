#pragma once

class Light;
class Mesh;

//To be added in the hopefully not so distant future
//This is just temporary. We should have a generic Collider class and specific implementations for
//different shapes
class BoxCollider
{

};

struct LightComponents
{
  Mesh* mesh = nullptr;
  BoxCollider* boxCollider = nullptr;
};

struct MeshComponents
{
  Light* light;
  BoxCollider* boxCollider;
};

