#include "guiEssentials.h"
#include "olcPixelGameEngine.h"
#include <string>

void DrawStringDropShadow(olc::PixelGameEngine* engine, const string& input, const olc::vi2d& location,const olc::vi2d& offsets, const olc::vi2d& scaling, const olc::Pixel& foreground, const olc::Pixel& background)
{
  olc::vf2d newPosition = location + offsets * scaling;
  engine->DrawStringDecal(location, input, background, scaling);
  engine->DrawStringDecal(newPosition, input, foreground, scaling);
}

void DrawStringDelay(olc::PixelGameEngine* engine, const string& input, const olc::vi2d& location, const olc::Pixel& color, const olc::vi2d& scaling, const float& delay, bool& isDone)
{
  if(isDone)
    return;
  static float currentDelay = delay;
  static int currentPosition = 1;
  currentDelay -= engine->GetElapsedTime();
  if(currentDelay <= 0)
  {
    engine->DrawStringDecal(location, input.substr(0, currentPosition), color, scaling);
    cout << "Printed: " << input.substr(0,currentPosition) << '\n';
    currentPosition++;
    currentDelay = delay;
  }
  
  if(currentPosition == input.size())
  {
    currentPosition = 0;
    currentDelay = delay;
    isDone = true;
  }
}

bool MouseInArea(olc::PixelGameEngine* engine, const olc::vi2d& boundary1, const olc::vi2d& boundary2)
{
  olc::vi2d mousePosition = engine->GetMousePos();
  if(mousePosition.x >= min(boundary1.x, boundary2.x) && mousePosition.x <= max(boundary1.x,boundary2.x) &&
     mousePosition.y >= min(boundary1.y, boundary2.y) && mousePosition.y <= max(boundary1.y, boundary2.y))
  {
    return true;
  }
  return false;
}

CheckBox::CheckBox(olc::PixelGameEngine* engine, void* manager, olc::Font* font, const string& text, const olc::vi2d& position, 
                   const olc::Pixel& fontColor, const olc::Pixel& enabledColor, const olc::Pixel& disabledColor, 
                   const float& angle, const olc::vi2d& padding, const bool& defaultState, const bool& enabled)
{
  this->font = font; this->text = text; this->engine = engine;
  this->position = position; 
  this->disabledColor = disabledColor; this->enabledColor = enabledColor; this->fontColor = fontColor;
  this->angle = angle; this->padding = padding;
  this->state = defaultState;
  this->isEnabled = enabled;
  static_cast<Manager*>(manager)->AddControl(this);
}

void CheckBox::Draw()
{
  if(isEnabled == false)
    return;

  u32string tempString(this->text.begin(), this->text.end());
  olc::FontRect fntRect = font->GetStringBounds(tempString);
  int stringHeight = fntRect.size.y;
  int stringLength = fntRect.size.x;
  
  //Firstly, fill in a rectangle color
  olc::vi2d rectangleSize = {0,0};
  rectangleSize.x += 2 * padding.x + stringLength;
  rectangleSize.y += 2 * padding.y + stringHeight;
  engine->FillRect(this->position.x, this->position.y, rectangleSize.x, rectangleSize.y, CHECK_COLOR);
  
  //Secondly draw the string 
  olc::vi2d stringPosition = {this->position.x + padding.x, this->position.y + padding.y + stringHeight};
  font->DrawString(tempString, stringPosition, this->fontColor, this->angle);
  this->topLeft = position;
  this->bottomRight = position + rectangleSize;
}

void CheckBox::Update()
{
  if(isEnabled == false)
    return;

  if(MouseInArea(this->engine, this->topLeft, this->bottomRight) && engine->GetMouse(0).bPressed)
  {
    this->state = !this->state;
  }
}

Manager::Manager()
{

}

Manager::~Manager()
{
  for(auto& guiPointer : this->guiElements)
  {
    delete guiPointer;
  }
  guiElements.clear();
}

void Manager::Update()
{
  for(auto& guiElm : guiElements)
  {
    static_cast<CheckBox*>(guiElm)->Update();
  }
}

void Manager::Draw()
{
  for(auto& guiElm : guiElements)
  {
    static_cast<CheckBox*>(guiElm)->Draw();
  }
}

void Manager::AddControl(void* guiPointer)
{
  this->guiElements.push_back(guiPointer);
}

void Manager::ChangeEnabledState(void* guiPointer, const bool& newState)
{
  for(auto& pointer : this->guiElements)
  {
    if(pointer == guiPointer)
    {
      static_cast<CheckBox*>(guiPointer)->isEnabled = newState;
    }
  }
}

void Manager::ChangeEnabledStatesExcept(void* guiPointer, const bool& newState)
{
  for(auto& pointer : this->guiElements)
  {
    if(pointer != guiPointer)
    {
      static_cast<CheckBox*>(guiPointer)->isEnabled = newState;
    }
  }
}
