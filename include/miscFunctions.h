#pragma once
#include <initializer_list>
#include <string>
#include "olcPixelGameEngine.h"

using namespace std;

olc::Pixel GetNoneMaterialColorCode(float luminance);
void MultiplyPixel(olc::Pixel& inputPixel, const float& valueToMultiply);
string GetExecutableDirectory(char* argvInput);
string GetLaunchPWD();
char GetPathSeperator();
//Interrupting in this case means that whether its exceptions should be handled or crash the program
filesystem::path GetPathObject(std::initializer_list<string> input, bool interupting = false);
string __CheckPathValidity(const filesystem::path& pathToCheck, bool interrupting, const string& failLocation); 
string GetPath(std::initializer_list<string> input = {}, bool interupting = false);
string GetPathFromResources(std::initializer_list<string> input = {}, bool interupting = false);
string GetPathFromConfig(std::initializer_list<string> input = {}, bool interupting = false);
string GetPathFromAudio(std::initializer_list<string> input = {}, bool interrupting = false);
string ConcatenatePaths(std::initializer_list<string> input);
void PrintColor(const olc::Pixel& color);
void WaitTillFPS(const float& fElapsedTime, const int& targetFPS);
u32string GetU32String(const string& stringInput);
string GetCurrentDate(bool twelveHour = false);
void TakeScreenshot(olc::PixelGameEngine* engine);
