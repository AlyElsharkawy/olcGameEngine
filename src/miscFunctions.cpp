#include <initializer_list>
#include <iomanip>
#include <string>
#include <filesystem>
#include <algorithm>
#include <thread>
#include <ctime>
#include "miscFunctions.h"
#include "globalVariables.h"
#include "stb_image_write.h"

using namespace std;

olc::Pixel GetNoneMaterialColorCode(float luminance)
{
  int temp = 255 * luminance;
  return olc::Pixel(clamp(temp, 0, 255), clamp(temp, 0 ,255), clamp(temp, 0, 255));
}

void MultiplyPixel(olc::Pixel& inputPixel, const float& valueToMultiply)
{
  inputPixel.r *= valueToMultiply;
  inputPixel.g *= valueToMultiply;
  inputPixel.b *= valueToMultiply;
  inputPixel.a *= valueToMultiply;
}

//LINUX ONLY
//Actually, its now cross platform. Thank you C++17 filesystem header!
string GetExecutableDirectory(char* argvInput) 
{
  /*char path[PATH_MAX];
  ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);

  if (count == -1) {
      // Handle error
      return "";
  }

  // Convert the path to a string
  string fullPath(path, count);

  // Find the last slash in the path
  size_t pos = fullPath.find_last_of("/");

  // Extract the directory part of the path
  return (string::npos == pos) ? "" : fullPath.substr(0, pos);*/ 

  //This is the path to the directory from which you launched the program from
  //This can vary cause you can supply a relative address if you launch the program from a terminal
  filesystem::path launchPath = filesystem::current_path() / filesystem::path(argvInput);
  try
  {
    return filesystem::canonical(launchPath.parent_path()).string();
  }
  catch(const filesystem::filesystem_error& e)
  {
    cerr << "FATAL ERROR: Failed to determine pwd of executable\n";
    cerr << "This is a fatal non-recoverable error. Please report to developer immiedtly. This should never happen!\n";
    throw;
  }
}

string GetLaunchPWD()
{
  return filesystem::current_path().string();
}

char GetPathSeperator()
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
  __CheckPathValidity(toReturn, interrupting, "PROGRAM_ROOT_DIRECTORY");
}

filesystem::path GetPathObject(std::initializer_list<string> input, bool interrupting)
{
  filesystem::path toReturn = PROGRAM_ROOT_DIRECTORY;
  for(const auto& elm : input)
  {
    toReturn /= elm;
  }
  
  __CheckPathValidity(toReturn, interrupting, "PROGRAM_ROOT_DIRECTORY");
}

string GetPathFromResources(std::initializer_list<string> input, bool interrupting)
{
  filesystem::path tempPath = GetPathObject({"..", "..", "resources"}, interrupting);
  for(const auto& elm : input)
  {
    tempPath /= elm;
  }
  
  __CheckPathValidity(tempPath, interrupting, "resources");
}

string GetPathFromConfig(std::initializer_list<string> input, bool interrupting)
{
  filesystem::path tempPath = GetPathObject({"..", "..", "resources","config"}, interrupting);
  for(const auto& elm : input)
  {
    tempPath /= elm;
  }
  
  __CheckPathValidity(tempPath, interrupting, "config");
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
    cout << "We will wait for " << timeToWaitMillis << " miliseconds\n";
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
