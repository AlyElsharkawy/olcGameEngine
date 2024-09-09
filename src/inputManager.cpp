#include <yaml-cpp/yaml.h>
#include <fstream>
#include "inputManager.h"
#include "miscFunctions.h"

bool InitializeInputs(olc::PixelGameEngine* engine, const string& yamlFile)
{
  YAML::Node root = YAML::LoadFile(GetPathFromConfig({"input.yaml"}));
  if(root.IsMap() == true)
  {
    for(auto it = root.begin(); it != root.end(); it++)
    {
      int indexTemp = it->first.as<int>();
      olc::Key indexKey = static_cast<olc::Key>(indexTemp);
      string value = it->second.as<string>();
      BASIC_CONTROLS[indexKey] = STRING_TO_KEY_LEGEND.at(value);
    }
  }

  else
  {
    cerr << "FATAL ERROR: Malformed input.yaml file. Aborting!\n";
    return false;
  }
  return true;
}

void WriteInitialInputs()
{
  YAML::Node root;
  root[to_string(MOVE_FORWARD)] = "W";
  root[to_string(MOVE_BACKWARD)] = "S";
  root[to_string(MOVE_LEFT)] = "A";
  root[to_string(MOVE_RIGHT)] = "D";
  root[to_string(MOVE_UP)] = "UP";
  root[to_string(MOVE_DOWN)] = "DOWN";
  root[to_string(LOOK_LEFT)] = "LEFT";
  root[to_string(LOOK_RIGHT)] = "RIGHT";
  root[to_string(ROTATE_UP)] = "P";
  root[to_string(ROTATE_DOWN)] = "L";
  root[to_string(JUMP)] = "SPACE";
  root[to_string(PAUSE)] = "ESCAPE";
  root[to_string(SPECIAL_ONE)] = "SHIFT";
  root[to_string(SPECIAL_TWO)] = "CTRL";
  root[to_string(SPECIAL_THREE)] = "DEL";
  root[to_string(SPECIAL_FOUR)] = "X"; //Placeholder. Alt isn't supported
  filesystem::path configFilePath = ConcatenatePaths({GetPathFromConfig(), "input.yaml"});
  cout << "The configFilePath is: " << configFilePath.string() << '\n';
  ofstream outputFile(configFilePath);
  outputFile << root;
  outputFile.close();
}

void InitializeInputMaps()
{
  YAML::Node root = YAML::LoadFile(GetPathFromConfig({"input.yaml"}));
  if(root.IsMap() == true)
  {
    //A to Z section
    int index = 1;
    while(index <= 26)
    {
      char temp = 'A' + (index - 1);
      olc::Key keyIndex = static_cast<olc::Key>(index);
      KEY_TO_STRING_LEGEND[keyIndex] = temp;
      STRING_TO_KEY_LEGEND[string(1,temp)] = keyIndex;
      index++;
    }
  }
  cout << "ALL ELEMENTS IN MAP:\n";
  for(auto it = STRING_TO_KEY_LEGEND.begin(); it != STRING_TO_KEY_LEGEND.end(); it++)
  {
    cout << it->first << " , " << it->second << '\n';
  }
}

void PrintPressedKeys(olc::PixelGameEngine* engine)
{
  vector<bool> isPressed(BASIC_CONTROLS.size(), false);
  int pressed = 0;
  for(int i = 0; i < isPressed.size(); i++)
  {
    if(engine->GetKey(BASIC_CONTROLS[i]).bPressed)
    {
      isPressed[i] = true;
      pressed++;
    }
  }
  
  if(pressed == 0)
    return;

  cout << "Currently pressed keys: \n";
  for(int i = 0; i < isPressed.size(); i++)
  {
    if(isPressed[i] == true)
    {
      cout << KEY_TO_STRING_LEGEND[BASIC_CONTROLS[i]] << ' ';
    }
  }
  cout << '\n';
}

