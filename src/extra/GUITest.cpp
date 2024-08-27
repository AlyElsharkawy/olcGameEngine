#define OLC_IMAGE_STB
#define OLC_PGE_APPLICATION
#define OLC_PGEX_QUICKGUI
#define OLC_PGEX_SPLASHSCREEN
#include <chrono>
#include "olcPixelGameEngine.h"
#include "olcPGEX_SplashScreen.h"
#include "olcPGEX_QuickGUI.h"
#include "guiEssentials.h"
#include "miscFunctions.h"

class Engine : public olc::PixelGameEngine
{
public:
  bool isDone;
  olc::QuickGUI::Manager mng;
  olc::SplashScreen sps;
  Engine()
  {
    sAppName = "GUITest";  
    isDone = false;
    mng = olc::QuickGUI::Manager();
  }
  bool OnUserCreate() override
  {
    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override
  {
    //Clear(olc::CYAN);
    auto frameBegin = chrono::steady_clock::now();
    //if(!isDone)
      //DrawStringDelay(this, "Hello, World!", {200,200}, olc::WHITE, {10,10}, 0.5f, isDone);
    DrawStringDropShadow(this, "Hello, World!", {100,100}, {1,1}, {10,10}, olc::WHITE, olc::BLACK);
    auto frameEnd = chrono::steady_clock::now();
    auto frameDuration = duration_cast<chrono::seconds>(frameEnd - frameBegin);
    olc::Sprite* spr = this->GetFontSprite();
    WaitTillFPS(static_cast<float>(frameDuration.count()), 144);
    return true;
  }
};

int main()
{
  Engine eng;
  eng.Construct(1920,1080,1,1);
  eng.Start();
}
