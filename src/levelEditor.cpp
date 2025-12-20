#include "levelEditor.h"
#include "globalVariables.h"
#include "imgui.h"
#include <fstream>
#include <iostream>
namespace fs = std::filesystem;

void LevelEditor::DrawUI(float fElapsedTime, MeshList &allObjects,
                         int32_t viewportTextureID) {
  static bool showFileBrowser = false;
  static std::string importedFilePath = "";
  static bool showAbout = false;
  // Main Menu
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Import")) {
        showFileBrowser = true;
      }
      if (ImGui::MenuItem("Exit", "ctrl + Q")) {
        this->isRunning = false;
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Help")) {
      if (ImGui::MenuItem("About")) {
        showAbout = true;
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  if (ImGui::Begin("Meshes")) {
    int meshIndex = 0;
    for (const auto &mesh : allObjects.GetMeshList()) {
      ImGui::PushID(meshIndex);
      std::string label =
          "Mesh " + std::to_string(meshIndex) + ": " + mesh->GetMeshName();
      if (ImGui::CollapsingHeader(label.c_str())) {
        ImGui::Text("Total Triangles: %d", mesh->GetTotalTriangles());
        ImGui::DragFloat3("Position", mesh->translationOffsets.data(), 0.1f);
        ImGui::DragFloat3("Rotation", mesh->rotationDegrees.data(), 0.1f);
        ImGui::DragFloat3("Scale", mesh->scalingOffsets.data(), 0.1f, 0.01f,
                          100.0f);
      }
      ImGui::PopID();
      meshIndex++;
    }
  }
  ImGui::End();
  DrawFileBrowser(&showFileBrowser, importedFilePath);
  if (!importedFilePath.empty()) {
    // std::cout << "Importing file: " << importedFilePath << std::endl;
    bool hasTexture = false;
    std::ifstream f(importedFilePath);
    if (f.is_open()) {
      std::string line;
      while (std::getline(f, line)) {
        if (line.length() > 2 && line[0] == 'f' && line[1] == ' ') {
          if (line.find('/') != std::string::npos) {
            hasTexture = true;
            break;
          }
        }
      }
      f.close();
    }
    // std::cout << "Has texture: " << hasTexture << std::endl;

    Mesh *newMesh = new Mesh();
    if (newMesh->LoadFromOBJFile(importedFilePath, hasTexture)) {
      // std::cout << "Mesh loaded successfully. Triangles: "
      // << newMesh->GetTotalTriangles() << std::endl;
      allObjects.AppendMesh(newMesh);
      allObjects.UpdateTotalCounts();
    } else {
      //   std::cout << "Failed to load mesh." << std::endl;
      delete newMesh;
    }
    importedFilePath = "";
  }
  if (showAbout) {
    ImGui::OpenPopup("About My Application");
  }

  // Always center the modal
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

  if (ImGui::BeginPopupModal("About My Application", &showAbout,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("3D Viewer Reborn");
    ImGui::Separator();
    ImGui::Text("Developed by: Aly Mohammed Elsharkawy");
    ImGui::Text("License: MIT");

    if (ImGui::Button("Close", ImVec2(120, 0))) {
      showAbout = false;
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
  ImGui::Begin("3D Viewer Reborn");
  ImGui::Text("Press ctrl + Q to exit the program.");
  ImGui::Text("Elapsed Time: %.3f ms", fElapsedTime * 1000.0f);
  ImGui::Text("FPS: %.2f", 1.0f / fElapsedTime);
  ImGui::End();

  ImGuiViewport *viewport = ImGui::GetMainViewport(); //
  ImGui::SetNextWindowPos(
      viewport->WorkPos); // Start at (0,0) or below menu bar
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  if (viewportTextureID != -1) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    if (ImGui::Begin("Viewport", NULL, window_flags)) {

      ImVec2 viewportSize = ImGui::GetContentRegionAvail();
      ImGui::Image((void *)(intptr_t)viewportTextureID, viewportSize);
    }
    ImGui::End();
    ImGui::PopStyleVar();
  }

  ImGui::Checkbox("Do Debug Menu", &SETTINGS_MAP[DO_DEBUG_MENU]);
  ImGui::Checkbox("Draw Lines", &SETTINGS_MAP[DRAW_LINES]);
  ImGui::Checkbox("Draw Faces", &SETTINGS_MAP[DRAW_FACES]);
  ImGui::Checkbox("Visualize Clipping", &SETTINGS_MAP[VISUALIZE_CLIPPING]);
  ImGui::Checkbox("Do Screen Space Clipping",
                  &SETTINGS_MAP[DO_SCREEN_SPACE_CLIPPING]);
  ImGui::Checkbox("Do View Space Clipping",
                  &SETTINGS_MAP[DO_VIEW_SPACE_CLIPPING]);
  ImGui::Checkbox("Draw Normals", &SETTINGS_MAP[DRAW_NORMALS]);
}
void LevelEditor::DrawFileBrowser(bool *open, std::string &selected_path) {
  if (!(*open))
    return;
  nfdchar_t *outPath = NULL;
  nfdresult_t result = NFD_OpenDialog(NULL, NULL, &outPath);

  if (result == NFD_OKAY) {
    // puts("Success!");
    // puts(outPath);
    selected_path = std::string(outPath);
    free(outPath);
    *open = false;
  } else if (result == NFD_CANCEL) {
    // puts("User pressed cancel.");
    *open = false;
  } else {
    // printf("Error: %s\n", NFD_GetError());
    *open = false;
  }
}
