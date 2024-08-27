#pragma once
#include <vector>
#include "olcPixelGameEngine.h"
#include "olcPGEX_TTF.h"

using namespace std;

//Inspiration taken from PGETinker
void DrawStringDropShadow(olc::PixelGameEngine* engine, const string& input, const olc::vi2d& location,const olc::vi2d& offsets, const olc::vi2d& scaling, const olc::Pixel& foreground, const olc::Pixel& background);
void DrawStringDelay(olc::PixelGameEngine* engine, const string& input, const olc::vi2d& location, const olc::Pixel& color, const olc::vi2d& scaling, const float& delay, bool& isDone);
bool MouseInArea(olc::PixelGameEngine* engine, const olc::vi2d& boundry1, const olc::vi2d& boundry2);

class CheckBox
{
private:
  olc::vi2d topLeft;
  olc::vi2d bottomRight;
public:
  olc::PixelGameEngine* engine;
  olc::Font* font;
  string text;
  olc::vi2d position;
  olc::Pixel color;
  olc::vi2d padding;
  bool isEnabled;
  float angle;
  bool state = false;
  CheckBox(olc::PixelGameEngine* engine, void* manager, olc::Font* font, const string& text, const olc::vi2d& position,const olc::Pixel& color = {255, 255, 255}, const float& angle = 0.0f, const olc::vi2d& padding = {10,10}, const bool& defaultState = true, const bool& enabled = true);
  void Draw();
  void Update();
};

class Manager
{
public:
  olc::Pixel normalColor = olc::BLUE;
  olc::Pixel hoverColor = olc::RED;
  olc::Pixel clickedColor = olc::YELLOW;
  Manager();
  ~Manager(); 
  void Update();
  void Draw();
  void ChangeEnabledState(void* guiPointer, const bool& newState);
  void ChangeEnabledStatesExcept(void* guiPointer, const bool& newState);
  void AddControl(void* guiPointer);

private:
  vector<void*> guiElements;
};


