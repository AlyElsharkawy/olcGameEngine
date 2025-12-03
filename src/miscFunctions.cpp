#include <initializer_list>
#include <iomanip>
#include <string>
#include <filesystem>
#include <algorithm>
#include <thread>
#include <ctime>
#include "miscFunctions.h"
#include "essentialFunctions.h"
#include "miscPrimitives.h"
#include "globalVariables.h"
#include "stb_image_write.h"

using namespace std;

#define EASY_PRINT_SIZE(primitiveName) cout << "Size of " #primitiveName ": " << sizeof(primitiveName) << " bytes\n"; 

olc::Pixel GetNoneMaterialColorCode(float luminance)
{
  int temp = 255 * luminance;
  return olc::Pixel(clamp(temp, 0, 255), clamp(temp, 0 ,255), clamp(temp, 0, 255));
}

void MultiplyPixelScalar(olc::Pixel& inputPixel, const float& valueToMultiply)
{
  inputPixel.r *= valueToMultiply;
  inputPixel.g *= valueToMultiply;
  inputPixel.b *= valueToMultiply;
}

olc::Pixel MultiplyPixelPixel(const olc::Pixel& pixel1, const olc::Pixel& pixel2)
{
  olc::Pixel toReturn;
  toReturn.r = COLOR_PARTIAL_MULTIPLY(pixel1.r, pixel2.r);
  toReturn.g = COLOR_PARTIAL_MULTIPLY(pixel1.g, pixel2.g);
  toReturn.b = COLOR_PARTIAL_MULTIPLY(pixel1.b, pixel2.b);
  return toReturn;
}

void MultiplyNormalizedPixelPixel(NormalizedPixel& pixel1, const NormalizedPixel& pixel2)
{
  pixel1.values[0] *= pixel2.values[0];
  pixel1.values[1] *= pixel2.values[1];
  pixel1.values[2] *= pixel2.values[2];
}

olc::Pixel ClampPixel(const NormalizedPixel& input)
{
  olc::Pixel toReturn;
  toReturn.r = clamp(input.values[0] * 255.0f, MINIMUM_DIFFUSE_COLOR, 255.0f);
  toReturn.g = clamp(input.values[1] * 255.0f, MINIMUM_DIFFUSE_COLOR, 255.0f);
  toReturn.b = clamp(input.values[2] * 255.0f, MINIMUM_DIFFUSE_COLOR, 255.0f);
  return toReturn;
}

NormalizedPixel SampleNormalizedPixel(const olc::Sprite* const texture, const float& x, const float& y)
{
  olc::Pixel toReturn = texture->Sample(x, y);
  float rVal, gVal, bVal;
  rVal = toReturn.r;
  gVal = toReturn.g;
  bVal = toReturn.b;
  rVal /= 255.0f;
  gVal /= 255.0f;
  bVal /= 255.0f;
  return NormalizedPixel(rVal, gVal, bVal);
}
//Actually, its now cross platform. Thank you C++17 filesystem header!
string GetExecutableDirectory(char* argvInput) 
{
  filesystem::path launchPath = filesystem::current_path() / filesystem::path(argvInput);
  try
  {
    return filesystem::canonical(launchPath.parent_path()).string();
  }
  catch(const filesystem::filesystem_error& e)
  {
    cerr << "FATAL ERROR: Failed to determine pwd of executable\n";
    cerr << "This is a fatal non-recoverable error. Please report to developer immiedtely. This should never happen!\n";
    throw;
  }
}

string GetLaunchPWD()
{
  return filesystem::current_path().string();
}

char GetPathSeparator()
{
  return char(filesystem::path::preferred_separator);
}

string __CheckPathValidity(const filesystem::path& pathToCheck, bool interrupting, const string& failLocation)
{
  try
  {
    return filesystem::canonical(pathToCheck).string();
  }

  catch(const filesystem::filesystem_error& e)
  {
    if(interrupting == true)
      throw;
    cerr << "Fatal error: Failed to resolve file path relative to program's" << failLocation << "variable" << '\n';
    cerr << e.what() << '\n';
    return "NULL_PATH";
  }
}

string GetPath(std::initializer_list<string> input, bool interrupting)
{
  filesystem::path toReturn = PROGRAM_ROOT_DIRECTORY;
  for(const auto& elm : input)
  {
    toReturn /= elm;
  }
  return __CheckPathValidity(toReturn, interrupting, "PROGRAM_ROOT_DIRECTORY");
}

filesystem::path GetPathObject(std::initializer_list<string> input, bool interrupting)
{
  filesystem::path toReturn = PROGRAM_ROOT_DIRECTORY;
  for(const auto& elm : input)
  {
    toReturn /= elm;
  }
  
  return __CheckPathValidity(toReturn, interrupting, "PROGRAM_ROOT_DIRECTORY");
}

string GetPathFromResources(std::initializer_list<string> input, bool interrupting)
{
  filesystem::path tempPath = GetPathObject({"..", "..", "resources"}, interrupting);
  for(const auto& elm : input)
  {
    tempPath /= elm;
  }
  
  return __CheckPathValidity(tempPath, interrupting, "resources");
}

string GetPathFromConfig(std::initializer_list<string> input)
{
  filesystem::path tempPath = GetPathObject({"..", "..", "resources","config"}, false);
  for(const auto& elm : input)
  {
    tempPath /= elm;
  }
  
  return std::filesystem::weakly_canonical(tempPath);
}

string GetPathFromAudio(std::initializer_list<string> input, bool interrupting)
{
  filesystem::path tempPath = GetPathObject({"..", "..", "resources","audio"}, interrupting);
  for(const auto& elm : input)
  {
    tempPath /= elm;
  }
  
  return __CheckPathValidity(tempPath, interrupting, "audio");
}

string ConcatenatePaths(std::initializer_list<string> input)
{
  filesystem::path toReturn;
  for(const auto& path : input)
  {
    toReturn /= path;
  }
  return toReturn.string();
}

void PrintColor(const olc::Pixel& color)
{
  cout << (short)color.r << ' ' << (short)color.g << ' ' << (short)color.b << '\n'; 
}

void WaitTillFPS(const float& fElapsedTime, const int& targetFPS)
{
  float desiredGameLoopTime = 1.0f / targetFPS;
  float timeToWait = desiredGameLoopTime - fElapsedTime;
  if(timeToWait >= 0)
  {
    int timeToWaitMillis = ceil(timeToWait * 1000.0f);
    cout << "We will wait for " << timeToWaitMillis << " milliseconds\n";
    this_thread::sleep_for(chrono::milliseconds(timeToWaitMillis));
  }
}

u32string GetU32String(const string& stringInput)
{
  return u32string(stringInput.begin(), stringInput.end());
}

string GetCurrentDate(bool twelveHour)
{
  std::time_t timeFrom1970 = time(nullptr);
  auto localTime = std::localtime(&timeFrom1970);

  std::ostringstream output;
  if(twelveHour == false)
    output << std::put_time(localTime, "%d-%m-%Y %H:%M:%S");
  else 
    output << std::put_time(localTime, "%d-%m-%Y %I:%M:%S %p");
  return output.str();
}

void TakeScreenshot(olc::PixelGameEngine* engine)
{
  cout << "Screenshot function called!\n";
  int screenWidth = engine->GetDrawTarget()->width;
  int screenHeight = engine->GetDrawTarget()->height;
  vector<olc::Pixel>& pixelBuffer = engine->GetDrawTarget()->pColData;
  vector<uint8_t> screenBuffer(screenWidth * screenHeight * 4);
  for(int y = 0; y < screenHeight; y++)
  {
    for(int x = 0; x < screenWidth; x++)
    {
      olc::Pixel currentPixel = pixelBuffer[y * screenWidth + x];
      int index = (y * screenWidth + x) * 4;
      screenBuffer[index + 0] = currentPixel.r;
      screenBuffer[index + 1] = currentPixel.g;
      screenBuffer[index + 2] = currentPixel.b;
      screenBuffer[index + 3] = currentPixel.a;
    }
  }
  string currentDate = GetCurrentDate();
  string name = string("screenshot ") + currentDate + string(".png");
  string path = ConcatenatePaths({GetPath({"..","..","screenshots"}), name});
  cout << "Screenshot Path: " << path << '\n';
  stbi_write_png(path.c_str(), screenWidth, screenHeight, 4, screenBuffer.data(), screenWidth * 4);
}

void HexToRGB(const std::string& hex, uint8_t& r, uint8_t& g, uint8_t& b)
{
    std::string h = hex;

    // Remove leading '#'
    if (!h.empty() && h[0] == '#')
        h.erase(0, 1);


    if (h.size() != 6)
    {
        r = 0; g = 0; b = 0;
        throw std::invalid_argument("Hex color must be characters.");
    }

    r = std::stoi(h.substr(0, 2), nullptr, 16);
    g = std::stoi(h.substr(2, 2), nullptr, 16);
    b = std::stoi(h.substr(4, 2), nullptr, 16);
}


void PrintGeometricPrimitivesSize()
{
  cout << "GEOMETRIC PRIMITIVES SIZE: " << '\n';
  EASY_PRINT_SIZE(Vector2D);
  EASY_PRINT_SIZE(Vector3D);
  EASY_PRINT_SIZE(Triangle);
  EASY_PRINT_SIZE(std::span<Vector2D>);
  EASY_PRINT_SIZE(std::span<Vector3D>);
  EASY_PRINT_SIZE(Mesh);
  cout << "\tMesh also contains 2 olc::Decal, 2 olc::Sprite, and 1 olc::Pixel pointers\n";
  EASY_PRINT_SIZE(MeshList);
  EASY_PRINT_SIZE(Matrix4x4);
  cout << '\n';
}

void PrintMiscellaneousPrimitivesSize()
{
  cout << "MISCELLANROUS PRIMITIVES SIZE: \n";
  EASY_PRINT_SIZE(Light);
  EASY_PRINT_SIZE(RenderingInstance);
  EASY_PRINT_SIZE(Camera);
  EASY_PRINT_SIZE(Player);
  cout << '\n';
}

void PrintOLCObjectsSize()
{
  cout << "OLC PRIMITIVES SIZE: " << '\n';
  EASY_PRINT_SIZE(olc::Pixel);
  EASY_PRINT_SIZE(olc::Decal);
  cout << "\tDecal also contains a olc::Sprite pointer\n"; 
  EASY_PRINT_SIZE(olc::Sprite);
  cout << '\n';
}

void PrintAllPrimitiveSizes()
{
  PrintGeometricPrimitivesSize();
  PrintMiscellaneousPrimitivesSize();
  PrintOLCObjectsSize();
}
