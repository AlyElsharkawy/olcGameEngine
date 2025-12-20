#include "levelEditor.h"
#include "globalVariables.h"
#include <fstream>
#include <iostream>
namespace fs = std::filesystem;

void LevelEditor::DrawUI(float fElapsedTime, MeshList &allObjects) {
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
    std::cout << "Importing file: " << importedFilePath << std::endl;
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
    std::cout << "Has texture: " << hasTexture << std::endl;

    Mesh *newMesh = new Mesh();
    if (newMesh->LoadFromOBJFile(importedFilePath, hasTexture)) {
      std::cout << "Mesh loaded successfully. Triangles: "
                << newMesh->GetTotalTriangles() << std::endl;
      allObjects.AppendMesh(newMesh);
      allObjects.UpdateTotalCounts();
    } else {
      std::cout << "Failed to load mesh." << std::endl;
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
  if (!*open)
    return;

  ImGui::Begin("File Browser", open);
  static fs::path current_dir = fs::current_path();

  if (ImGui::Button("..")) { // Go up a directory
    current_dir = current_dir.parent_path();
  }

  ImGui::Separator();

  for (const auto &entry : fs::directory_iterator(current_dir)) {
    auto path = entry.path();
    std::string label = path.filename().string();

    if (entry.is_directory()) {
      if (ImGui::Selectable(("[Dir] " + label).c_str())) {
        current_dir = path;
      }
    } else {
      if (ImGui::Selectable(label.c_str())) {
        selected_path = path.string();
        *open = false; // Close browser after selection
      }
    }
  }
  ImGui::End();
}
