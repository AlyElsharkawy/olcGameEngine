#include <string>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <vector>
#include "inputManager.h"
#include "miscFunctions.h"

InputManager::InputManager()
{
  //The keypad had to be added to give applications a way to differentiate between keypad and numpad
  //number presses
  this->KEY_TO_STRING_LEGEND = 
  unordered_map<olc::Key, string>
  {
    {olc::Key::UP, "UP"}, {olc::Key::DOWN, "DOWN"},
    {olc::Key::LEFT, "LEFT"}, {olc::Key::RIGHT, "RIGHT"},
    {olc::Key::CTRL, "CTRL"}, {olc::Key::SHIFT, "SHIFT"}, {olc::Key::DEL, "DEL"},
    {olc::Key::SPACE, "SPACE"} , {olc::Key::ESCAPE, "ESCAPE"},
    {olc::Key::F1, "F1"}, {olc::Key::F2, "F2"}, {olc::Key::F3, "F3"}, {olc::Key::F4, "F4"},
    {olc::Key::F5, "F5"}, {olc::Key::F6, "F6"}, {olc::Key::F7, "F7"}, {olc::Key::F8, "F8"},
    {olc::Key::F9, "F9"}, {olc::Key::F10, "F10"}, {olc::Key::F11, "F11"}, {olc::Key::F12, "F12"},
    {olc::Key::K1, "K1"}, {olc::Key::K2, "K2"}, {olc::Key::K3, "K3"}, {olc::Key::K4, "K4"}, {olc::Key::K5, "K5"},
    {olc::Key::K6, "K6"}, {olc::Key::K7, "K7"}, {olc::Key::K8, "K8"}, {olc::Key::K9, "K9"}, {olc::Key::K0, "K0"},
    {olc::Key::NP1, "NP1"}, {olc::Key::NP2, "NP2"}, {olc::Key::NP3, "NP3"}, {olc::Key::NP4, "NP4"}, {olc::Key::NP5, "NP5"},
    {olc::Key::NP6, "NP6"}, {olc::Key::NP7, "NP7"}, {olc::Key::NP8, "NP8"}, {olc::Key::NP9, "NP9"}, {olc::Key::NP0, "NP0"},
  };
  
  this->STRING_TO_KEY_LEGEND =
  unordered_map<string, olc::Key>
  {
    {"UP", olc::Key::UP}, {"DOWN", olc::Key::DOWN},
    {"LEFT", olc::Key::LEFT} , {"RIGHT", olc::Key::RIGHT},
    {"CTRL", olc::Key::CTRL} , {"SHIFT", olc::Key::SHIFT}, {"DEL", olc::Key::DEL},
    {"SPACE", olc::Key::SPACE}, {"ESCAPE", olc::Key::ESCAPE},
    {"F1", olc::Key::F1}, {"F2", olc::Key::F2}, {"F3", olc::Key::F3}, {"F4", olc::Key::F4},
    {"F5", olc::Key::F5}, {"F6", olc::Key::F6}, {"F7", olc::Key::F7}, {"F8", olc::Key::F8},
    {"F9", olc::Key::F9}, {"F10", olc::Key::F10}, {"F11", olc::Key::F11}, {"F12", olc::Key::F12},
    {"K1", olc::Key::K1}, {"K2", olc::Key::K2}, {"K3", olc::Key::K3}, {"K4", olc::Key::K4}, {"K5", olc::Key::K5},
    {"K6", olc::Key::K6}, {"K7", olc::Key::K7}, {"K8", olc::Key::K8}, {"K9", olc::Key::K9}, {"K0", olc::Key::K0},
    {"NP1", olc::Key::NP1}, {"NP2", olc::Key::NP2}, {"NP3", olc::Key::NP3}, {"NP4", olc::Key::NP4}, {"NP5", olc::Key::NP5},
    {"NP6", olc::Key::NP6}, {"NP7", olc::Key::NP7}, {"NP8", olc::Key::NP8}, {"NP9", olc::Key::NP9}, {"NP0", olc::Key::NP0},
  };
  
  this->BASIC_CONTROLS = vector<olc::Key>(BASIC_CONTROLS_SIZE);
  WriteInitialInputs();
  InitializeInputMaps();
}

InputManager& InputManager::Get()
{
  static InputManager instance;
  return instance;
}

void InputManager::InitializeInputMaps()
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
      this->KEY_TO_STRING_LEGEND[keyIndex] = temp;
      this->STRING_TO_KEY_LEGEND[string(1,temp)] = keyIndex;
      index++;
    }
  }
  #ifdef BUILD_DEBUG
    cout << "ALL ELEMENTS IN MAP:\n";
    for(auto it = this->STRING_TO_KEY_LEGEND.begin(); it != this->STRING_TO_KEY_LEGEND.end(); it++)
    {
      cout << it->first << " , " << it->second << '\n';
    }
    cout << '\n';
  #endif
}

bool InputManager::InitializeInputs(const string& yamlFile)
{
  Get().WriteInitialInputs();
  YAML::Node root = YAML::LoadFile(GetPathFromConfig({"input.yaml"}));
  if(root.IsMap() == true)
  {
    for(auto it = root.begin(); it != root.end(); it++)
    {
      int indexTemp = it->first.as<int>();
      olc::Key indexKey = static_cast<olc::Key>(indexTemp);
      string value = it->second.as<string>();
      Get().BASIC_CONTROLS[indexKey] = Get().STRING_TO_KEY_LEGEND.at(value);
    }
  }

  else
  {
    cerr << "FATAL ERROR: Malformed input.yaml file. Aborting!\n";
    return false;
  }
  return true;
}

void InputManager::WriteInitialInputs()
{
  if(filesystem::exists(GetPathFromConfig() + "/input.yaml"))
    return;

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

  //Numeric keys (top row of keyboard)
  root[to_string(NUM_1)] = "K1";
  root[to_string(NUM_2)] = "K2";
  root[to_string(NUM_3)] = "K3";
  root[to_string(NUM_4)] = "K4";
  root[to_string(NUM_5)] = "K5";
  root[to_string(NUM_6)] = "K6";
  root[to_string(NUM_7)] = "K7";
  root[to_string(NUM_8)] = "K8";
  root[to_string(NUM_9)] = "K9";
  root[to_string(NUM_0)] = "K0";

  //Numpad keys (side of keyboard)
  root[to_string(NUMPAD_1)] = "NP1";
  root[to_string(NUMPAD_2)] = "NP2";
  root[to_string(NUMPAD_3)] = "NP3";
  root[to_string(NUMPAD_4)] = "NP4";
  root[to_string(NUMPAD_5)] = "NP5";
  root[to_string(NUMPAD_6)] = "NP6";
  root[to_string(NUMPAD_7)] = "NP7";
  root[to_string(NUMPAD_8)] = "NP8";
  root[to_string(NUMPAD_9)] = "NP9";
  root[to_string(NUMPAD_0)] = "NP0";

  //root[to_string(SPECIAL_FOUR)] = "X"; //Placeholder. Alt isn't supported
  filesystem::path configFilePath = ConcatenatePaths({GetPathFromConfig(), "input.yaml"});
  cout << "The configFilePath is: " << configFilePath.string() << '\n';
  ofstream outputFile(configFilePath);
  outputFile << root;
  outputFile.close();
}

void InputManager::PrintPressedKeys(olc::PixelGameEngine* engine)
{
  vector<bool> isPressed(Get().BASIC_CONTROLS.size(), false);
  int pressed = 0;
  for(int i = 0; i < isPressed.size(); i++)
  {
    if(engine->GetKey(Get().BASIC_CONTROLS[i]).bPressed)
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
      cout << Get().KEY_TO_STRING_LEGEND[Get().BASIC_CONTROLS[i]] << ' ';
    }
  }
  cout << '\n';
}

bool InputManager::KeyHeld(olc::PixelGameEngine* engine, std::initializer_list<int> input)
{
  for(auto it = input.begin(); it != input.end(); it++)
  {
    auto& elmToCheck = Get().BASIC_CONTROLS[*it];
    if(engine->GetKey(elmToCheck).bHeld == false)
      return false;
  }
  return true;
}

bool InputManager::KeyPressed(olc::PixelGameEngine* engine, int input)
{
  auto& elmToCheck = Get().BASIC_CONTROLS[input];
  return engine->GetKey(elmToCheck).bPressed;
}

bool InputManager::KeyReleased(olc::PixelGameEngine* engine ,std::initializer_list<int> input)
{
  for(auto it = input.begin(); it != input.end(); it++)
  {
    auto& elmToCheck = Get().BASIC_CONTROLS[*it];
    if(engine->GetKey(elmToCheck).bReleased == false)
      return false;
  }
  return true;
}

bool InputManager::ReassignKey(olc::Key input, const string& newKey)
{
  try
  {
    olc::Key newInput = Get().STRING_TO_KEY_LEGEND.at(newKey);
    Get().BASIC_CONTROLS[input] = newInput;
  }
  catch(const exception& e)
  {
    return false;
  }
}

