#define OLC_PGE_APPLICATION
#include "globalVariables.h"
#include "olcPixelGameEngine.h"
#include "serialization.h"
#include "miscFunctions.h"

int main(int argc, char** argv)
{
  PROGRAM_ROOT_DIRECTORY = GetExecutableDirectory(argv[0]);
  Light* toSerialize = new Light(0, {0,0,0}, {2,2,2}, "myLight", {255,255,255}, 1.0f);
  SerializeLight(toSerialize, GetPathFromResources({"misc", "testLight1.lightdata"}));

  Light* toDeserialize = DeserializeLight(GetPathFromResources({"misc", "testLight1.lightdata"}));
  toDeserialize->PrintLightInfo();

  Mesh* bunny = new Mesh();
  bunny->LoadFromOBJFile(GetPathFromResources({"objectFiles", "Primitives", "bunny.obj"}));
  bunny->doAutomaticRotation = true;
  bunny->doAutomaticRotations[1] = true;
  bunny->SetTranslationOffsets(0, 0, 9);
  bunny->SetScalingOffsets(60, 60, 60);
  uint8_t rVal, bVal, gVal;
  HexToRGB("d5e1f0", rVal, gVal, bVal);
  bunny->SetDiffuseColor(rVal, gVal, bVal, 255);
  bunny->SetMeshName("My beautiful bunny");
  SerializeMesh(bunny, GetPathFromResources({"misc", "beautiful-bunny.mesh"}), 
                "./objectFiles/bunny.mesh", "./textures/bunnyTexture.png");

  bunny->PrintMeshInfo();

  Mesh* importedBunny = DeserializeMesh(GetPathFromResources({"misc", "beautiful-bunny.mesh"}));
  if(importedBunny != nullptr)
    importedBunny->PrintMeshInfo();

  Mesh* importedBunny2 = DeserializeMesh(GetPathFromResources({"misc", "ugly-bunny.mesh"}));
  if(importedBunny2 != nullptr)
    importedBunny2->PrintMeshInfo();
  return 0;
}
