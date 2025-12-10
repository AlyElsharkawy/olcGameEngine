#define OLC_PGE_APPLICATION
#define OLC_IMAGE_STB

#include <iostream>
#include <fstream>
#include "geometricPrimitives.h"
#include "miscFunctions.h"

using namespace std;

int main(int argc, char** argv)
{
  Mesh loadedObject;
  bool isLoaded;
  while(true)
  {
    string fileToLoad;
    cout << "Enter file to load(without extension): "; cin >> fileToLoad;
    isLoaded = loadedObject.LoadFromOBJFile(GetPathFromResources({"objectFiles", fileToLoad}));
    if(isLoaded == false)
    {
      cerr << "OBJ File failed to load.\n";
      continue;
    }

    ofstream outputFile(GetExecutableDirectory(argv[0]) + fileToLoad + ".txt");
    if(!outputFile.is_open())
    {
      cerr << "Failed to open file\n";
      continue;
    }

    for(int i = 0; i < loadedObject.GetTriangles().size(); i++)
    {
      cout << loadedObject.GetTriangles()[i].ExtractInfo() << ",\n";
      outputFile << loadedObject.GetTriangles()[i].ExtractInfo() << ",\n";
      if(i == loadedObject.GetTriangles().size() - 1)
      {
        cout << loadedObject.GetTriangles()[i].ExtractInfo() << "\n";
        outputFile << loadedObject.GetTriangles()[i].ExtractInfo() << "\n";
      }
    }
    outputFile.close();
  }
}
