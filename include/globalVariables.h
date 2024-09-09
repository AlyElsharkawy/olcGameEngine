#pragma once
#include "geometricPrimitives.h"

enum SETTINGS_ENUM : uint8_t
{
  DRAW_NORMALS,
  DRAW_LINES,
  DRAW_FACES,

  DO_SPLASH_SCREEN,
  DO_PERFORMANCE_CLEARING,

  DO_DEBUG_MENU,

  VISUALIZE_CLIPPING,
  DO_VIEW_SPACE_CLIPPING,
  DO_SCREEN_SPACE_CLIPPING,
};

inline unordered_map<uint8_t,bool> SETTINGS_MAP
{
  {DRAW_NORMALS, false},
  {DRAW_LINES , true},
  {DRAW_FACES, true},

  {DO_SPLASH_SCREEN, false},

  {DO_PERFORMANCE_CLEARING, false},
  
  {DO_DEBUG_MENU, true},

  {VISUALIZE_CLIPPING, false},
  {DO_VIEW_SPACE_CLIPPING, true},
  {DO_SCREEN_SPACE_CLIPPING, true},
};

//Composite rotations like XZ, ZX, XY may be added in the future
//But frankly, I don't see a reason for their existance now
enum ROT_TYPES : short
{
  ROT_X,
  ROT_Y,
  ROT_Z,
  ROT_ZYX,
};

//Composite will be implemented similairly to the mix shader from blender
//None Material: The mesh will be colored black or white, depending on the intensity of surrounding
//lights. Black means no light hit the mesh while white means the maximum amount of light hit the
//mesh
//Diffuse Material: The mesh will have a color and will be affected by the color and intensity of
//surrounding lights
enum MATERIAL_TYPES : short
{
  NONE, //Black and white colors
  DIFFUSE, //Standard 1 color materials
  TEXTURE, //Texturing from PNGs
  TEXTURE_WITH_NORMAL, //Texturing with normal maps
  COMPOSITE, //Mix of any previous material types
};

//LIGHT prefix is cause windows reserves the POINT data type. This is so that you can use this project
//on windows
enum LIGHT_TYPES : short
{
  LAMP_SUN,
  LAMP_POINT,
};

const inline float mathPI = 3.14159f;
inline float ASPECT_RATIO;

const inline float SUN_DIVISION_CONSTANT = 1/100.0f;
const inline float MINIMUM_NONE_LUMINANCE = 0.6f;

const inline float MINIMUM_DIFFUSE_LUMINANCE = 0.3f;
const inline float MINIMUM_DIFFUSE_COLOR = 10.0f;

const inline float EPSILON = 1e-6f;

//These are default values for the miscPrimitives.cpp file
const inline float MINIMUM_HORIZONTAL_SPEED = 1.0f;
const inline float MINIMUM_VERTICAL_SPEED = 1.0f;
const inline float MINIMUM_ROTATION_SPEED = 30.0f;
const inline float MINIMUM_NEAR_DISTANCE = 0.1f;
const inline float MINIMUM_FAR_DISTANCE = 1000.0f;
const inline float DEFAULT_FOV = 90.0f;
const inline Vector3D DEFAULT_POSITION = {0.0f,0.0f, 0.0f};
const inline Vector3D DEFAULT_FACING_VECTOR = {0.0f, 0.0f, 1.0f};

const inline Vector3D UP_DIRECTION = {0.0f, 1.0f, 0.0f};

const inline olc::Sprite* MISSING_TEXTURE_SPRITE;

const olc::Pixel WIREFRAME_COLOR = olc::BLUE;

inline string PROGRAM_ROOT_DIRECTORY;
