#include <initializer_list>
#include <string>
#include <filesystem>
#include <algorithm>
#include <thread>
#include "miscFunctions.h"
#include "globalVariables.h"

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
    return filesystem::canonical(launchPath.parent_path());
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
  return string(filesystem::current_path());
}

char GetPathSeperator()
{
  return char(filesystem::path::preferred_separator);
}

string GetPath(std::initializer_list<string> input, bool interrupting)
{
  filesystem::path toReturn = PROGRAM_ROOT_DIRECTORY;
  for(const auto& elm : input)
  {
    toReturn /= elm;
  }

  try
  {
    return filesystem::canonical(toReturn);
  }

  catch(const filesystem::filesystem_error& e)
  {
    if(interrupting == true)
      throw;
    cerr << "Fatal error: Failed to resolve file path relative to program PROGRAM_ROOT_DIRECTORY variable" << '\n';
    cerr << e.what() << '\n';
    return "NULL_PATH";
  }
}

filesystem::path GetPathObject(std::initializer_list<string> input, bool interrupting)
{
  filesystem::path toReturn = PROGRAM_ROOT_DIRECTORY;
  for(const auto& elm : input)
  {
    toReturn /= elm;
  }
  
  try
  {
    return filesystem::canonical(toReturn);
  }

  catch(const filesystem::filesystem_error& e)
  {
    if(interrupting == true)
      throw;
    cerr << "Fatal error: Failed to resolve file path relative to program PROGRAM_ROOT_DIRECTORY variable" << '\n';
    cerr << e.what() << '\n';
    return "NULL_PATH";
  }
}

string GetPathFromResources(std::initializer_list<string> input, bool interrupting)
{
  filesystem::path tempPath = GetPathObject({"..", "..", "resources"}, interrupting);
  for(const auto& elm : input)
  {
    tempPath /= elm;
  }

  try
  {
    return filesystem::canonical(tempPath);
  }

  catch(const filesystem::filesystem_error& e)
  {
    if(interrupting == true)
      throw;
    cerr << "Fatal error: Failed to resolve file path relative to program resources folder" << '\n';
    cerr << e.what() << '\n';
    return "NULL_PATH";
  }
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


