#pragma once
#include <unordered_map>
#include <vector>
#include <initializer_list>
#include "olcPixelGameEngine.h"

using namespace std;

enum BASIC_CONTROLS_ENUM
{
  //X-Z movement
  MOVE_FORWARD,
  MOVE_BACKWARD,
  MOVE_LEFT,
  MOVE_RIGHT,

  //Y movement
  MOVE_UP,
  MOVE_DOWN,
  
  //Y rotation
  LOOK_LEFT,
  LOOK_RIGHT,

  //Local X rotation
  ROTATE_UP,
  ROTATE_DOWN,

  JUMP,
  PAUSE,

  //Special keys
  SPECIAL_ONE, //e.g shift
  SPECIAL_TWO, //e.g control
  SPECIAL_THREE, //e.g enter
  SPECIAL_FOUR, //placeholder. Alt isn't supported
  BASIC_CONTROLS_SIZE, //Will always contain the size of the enum
};

class InputManager
{
private:
  InputManager(const InputManager&) = delete;
  InputManager& operator=(const InputManager&) = delete;

  InputManager();
  vector<olc::Key> BASIC_CONTROLS;
  
  //These are used internally to translate olc::Keys to string and vice versa
  unordered_map<olc::Key, string> KEY_TO_STRING_LEGEND;
  unordered_map<string, olc::Key> STRING_TO_KEY_LEGEND;
  void WriteInitialInputs();
  void InitializeInputMaps();

public:
  static InputManager& Get();
  static void PrintPressedKeys(olc::PixelGameEngine *engine);
  static bool InitializeInputs(const string& yamlFile);
  static bool KeyPressed(olc::PixelGameEngine* engine, int input);
  static bool KeyHeld(olc::PixelGameEngine* engine, std::initializer_list<int> input);
  static bool KeyReleased(olc::PixelGameEngine* engine, std::initializer_list<int> input);
  static void ReassignKey(olc::Key input, const string& newKey);
};
