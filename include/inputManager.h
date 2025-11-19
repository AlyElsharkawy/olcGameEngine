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

  //Miscellaneous keys. These are intended to be used for special abilities and such
  MISC_1,
  MISC_2,
  MISC_3,
  MISC_4,
  MISC_5,

  //Numeric keys (top row of keyboard)
  NUM_1,
  NUM_2,
  NUM_3,
  NUM_4,
  NUM_5,
  NUM_6,
  NUM_7,
  NUM_8,
  NUM_9,
  NUM_0,

  //Numpad keys (side of keyboard)
  NUMPAD_1,
  NUMPAD_2,
  NUMPAD_3,
  NUMPAD_4,
  NUMPAD_5,
  NUMPAD_6,
  NUMPAD_7,
  NUMPAD_8,
  NUMPAD_9,
  NUMPAD_0,

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
  static bool ReassignKey(olc::Key input, const string& newKey);
};
