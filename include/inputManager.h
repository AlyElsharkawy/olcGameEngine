#pragma once
#include <cstdint>
#include <unordered_map>
#include <vector>
#include "olcPixelGameEngine.h"

using namespace std;

enum BASIC_CONTROLS_ENUM : uint8_t
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
  SIZE, //Will always contain the size of the enum
};

//These are used internally to translate olc::Keys to string and vice versa
inline unordered_map<string, olc::Key> STRING_TO_KEY_LEGEND
{
  {"UP", olc::Key::UP}, {"DOWN", olc::Key::DOWN},
  {"LEFT", olc::Key::LEFT} , {"RIGHT", olc::Key::RIGHT},
  {"CTRL", olc::Key::CTRL} , {"SHIFT", olc::Key::SHIFT}, {"SHIFT", olc::Key::SHIFT}, {"DEL", olc::Key::DEL},
  {"SPACE", olc::Key::SPACE}, {"ESCAPE", olc::Key::ESCAPE},
  {"F1", olc::Key::F1}, {"F2", olc::Key::F2}, {"F3", olc::Key::F3}, {"F4", olc::Key::F4},
  {"F5", olc::Key::F5}, {"F6", olc::Key::F6}, {"F7", olc::Key::F7}, {"F8", olc::Key::F8},
  {"F9", olc::Key::F9}, {"F10", olc::Key::F10}, {"F11", olc::Key::F11}, {"F12", olc::Key::F12},
};

inline unordered_map<olc::Key, string> KEY_TO_STRING_LEGEND
{
  {olc::Key::UP, "UP"}, {olc::Key::DOWN, "DOWN"},
  {olc::Key::LEFT, "LEFT"}, {olc::Key::RIGHT, "RIGHT"},
  {olc::Key::CTRL, "CTRL"}, {olc::Key::SPACE, "SPACE"}, {olc::Key::SHIFT, "SHIFT"}, {olc::Key::DEL, "DEL"},
  {olc::Key::SPACE, "SPACE"} , {olc::Key::ESCAPE, "ESCAPE"},
  {olc::Key::F1, "F1"}, {olc::Key::F2, "F2"}, {olc::Key::F3, "F3"}, {olc::Key::F4, "F4"},
  {olc::Key::F5, "F5"}, {olc::Key::F6, "F6"}, {olc::Key::F7, "F7"}, {olc::Key::F8, "F8"},
  {olc::Key::F9, "F9"}, {olc::Key::F10, "F10"}, {olc::Key::F11, "F11"}, {olc::Key::F12, "F12"},
};

inline vector<olc::Key> BASIC_CONTROLS(BASIC_CONTROLS_ENUM::SIZE);
bool InitializeInputs(olc::PixelGameEngine* engine, const string& yamlFile);
void WriteInitialInputs();
void InitializeInputMaps();
void PrintPressedKeys(olc::PixelGameEngine* engine);
