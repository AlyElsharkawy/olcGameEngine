#pragma once
#include "geometricPrimitives.h"
#include "imgui.h"
#include "imgui_impl_pge.h"
#include "olcPixelGameEngine.h"
#include <filesystem>
#include <string>
class LevelEditor {
public:
  void DrawFileBrowser(bool *open, std::string &selected_path);
  void DrawUI(float fElapsedTime, MeshList &allObjects);

private:
  bool isRunning = false;
};
