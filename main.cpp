#define OLC_PGE_APPLICATION
#define OLC_IMAGE_STB
#define OLC_PGEX_CUSTOMFONT
#define OLC_PGEX_FONT
#define OLC_PGEX_SPLASHSCREEN
#define OLC_PGEX_TTF
#define OLC_PGEX_MINIAUDIO
#define OLC_PGEX_DEAR_IMGUI_IMPLEMENTATION
#include "audioManager.h"
#include "clippingRoutines.h"
#include "drawingRoutines.h"
#include "essentialFunctions.h"
#include "geometricPrimitives.h"
#include "globalVariables.h"
#include "guiEssentials.h"
#include "imgui.h"
#include "imgui_impl_pge.h"
#include "inputManager.h"
#include "matrixMathEssentials.h"
#include "miscFunctions.h"
#include "miscPrimitives.h"
#include "normalMathEssentials.h"
#include "olcPGEX_SplashScreen.h"
#include "olcPixelGameEngine.h"
#include "timerUtility.h"
#include "triangleMathEssentials.h"
#include "vectorMathEssentials.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nfs/nfs.h>
#include <string>
namespace fs = std::filesystem;

#define DEFAULT_COLORS olc::BLUE, olc::GREEN, olc::RED

using namespace std;

unsigned long long totalFPS = 0;
unsigned long long totalTimes = 0;

class EngineReborn : public olc::PixelGameEngine {
  // Splash screen. Initializing it causes it to occur
  bool isRunning = false;
  olc::SplashScreen *sps = nullptr;
  float *depthBuffer = nullptr;
  int m_GameLayer;
  olc::imgui::PGE_ImGUI pge_imgui;
  // GUI related stuff
  Manager manager;
  bool showAbout = false;
  // Custom fonts
  olc::Font *fontFreeSans = nullptr;
  olc::Font *fontFreeSansBold = nullptr;
  olc::Font *fontHackButtons = nullptr;
  bool showFileBrowser = false;
  std::string importedFilePath = "";
  // Rendering instance used to efficiently pass information between functions
  // and phases
  RenderingInstance RI;

public:
  EngineReborn() {
    sAppName = "3D Viewer Reborn";
#ifdef BUILD_RELEASE
    sps = new olc::SplashScreen();
#endif
  }

public:
  // Object variables
  MeshList allObjects;
  deque<Light *> allLights;
  float totalElapsedTime = 0.0f;
  Matrix4x4 zRotMat = GetIdentityMatrix();
  Matrix4x4 yRotMat = GetIdentityMatrix();
  Matrix4x4 xRotMat = GetIdentityMatrix();
  Player *player;

  // Consideration: Should this be another data structure?
  vector<Triangle> trianglesToRaster;
  vector<pair<Vector3D, Vector3D>> normalsToRaster;
  deque<Triangle> screenSpaceClippedTriangles;
  int trianglesRasteredCount = 0;
  // Artifact from bug number 2 hunting
  // vector<Triangle> preClipTris;

  bool OnUserCreate() override {
    // Initializing ASPECT_RATIO
    ASPECT_RATIO = (float)ScreenHeight() / (float)ScreenWidth();

    // Initializing fonts
    olc::Font::init();

    InputManager::InitializeInputs(GetPathFromConfig({"input.yaml"}));

    string pathToScreenshots = GetPath({
                                   PROGRAM_ROOT_DIRECTORY,
                                   "..",
                                   "..",
                               }) +
                               string("/screenshots");
    if (filesystem::exists(pathToScreenshots) == false) {
      cout << "Screenshots directory does not exist. Creating it now...\n";
      filesystem::create_directory(pathToScreenshots);
    }

    // Initialize single player instance
    player = new Player();
    player->camera.CalculateProjectionMatrix();
    player->camera.SetMovementSpeeds(10.0f, 10.0f, 150.0f);

    // Initialize missing texture sprite. Will crash if fails
    MISSING_TEXTURE_SPRITE = new olc::Sprite(
        GetPathFromResources({"textures", "missingTexture.png"}));
    // Initialize RenderingInstance
    RI.InitializeRenderingInstance(this);

    // Initializing Custom fonts
    fontFreeSans = new olc::Font(
        GetPathFromResources({"fonts", "TTF", "Hack-Regular.ttf"}), 50);
    fontFreeSansBold = new olc::Font(
        GetPathFromResources({"fonts", "TTF", "FreeSansBold.ttf"}), 50);
    fontHackButtons = new olc::Font(
        GetPathFromResources({"fonts", "TTF", "FreeSans.ttf"}), 25);

    // Initialize GUI elements
    // checkDoDebugMenu = new CheckBox(
    //     this, &manager, fontHackButtons, "Do Debug Menu", {0, 420},
    //     DEFAULT_COLORS, 0.0f, {20, 20}, SETTINGS_MAP[DO_DEBUG_MENU]);
    // checkDrawLines =
    //     new CheckBox(this, &manager, fontHackButtons, "Draw Lines", {0, 500},
    //                  DEFAULT_COLORS, 0.0f, {20, 20},
    //                  SETTINGS_MAP[DRAW_LINES]);
    // checkDrawFaces =
    //     new CheckBox(this, &manager, fontHackButtons, "Draw Faces", {0, 580},
    //                  DEFAULT_COLORS, 0.0f, {20, 20},
    //                  SETTINGS_MAP[DRAW_FACES]);
    // checkVisualizeClipping = new CheckBox(
    //     this, &manager, fontHackButtons, "Visualize Clipping", {0, 660},
    //     DEFAULT_COLORS, 0.0f, {20, 20}, SETTINGS_MAP[VISUALIZE_CLIPPING]);
    // checkDoScreenSpaceClipping = new CheckBox(
    //     this, &manager, fontHackButtons, "Do Screen Space Clipping", {0,
    //     740}, DEFAULT_COLORS, 0.0f, {20, 20},
    //     SETTINGS_MAP[DO_SCREEN_SPACE_CLIPPING]);
    // checkDoViewSpaceClipping = new CheckBox(
    //     this, &manager, fontHackButtons, "Do View Space Clipping", {0, 820},
    //     DEFAULT_COLORS, 0.0f, {20, 20},
    //     SETTINGS_MAP[DO_VIEW_SPACE_CLIPPING]);
    // checkDrawNormals = new CheckBox(
    //     this, &manager, fontHackButtons, "Draw Normals", {0, 900},
    //     DEFAULT_COLORS, 0.0f, {20, 20}, SETTINGS_MAP[DRAW_NORMALS], true);
    // checkShowOptionsMenu =
    //     new CheckBox(this, &manager, fontHackButtons, "Show Options", {0,
    //     980},
    //                  DEFAULT_COLORS, 0.0f, {20, 20}, true);
    // Create a new Layer which will be used for the game
    m_GameLayer = CreateLayer();
    EnableLayer(m_GameLayer, true);
    SetLayerCustomRenderFunction(0, std::bind(&EngineReborn::DrawUI, this));
    SetInitialObjects(this, allObjects, allLights, NUM_1);
#ifdef BUILD_DEBUG
    PrintAllPrimitiveSizes();
#endif
    this->isRunning = true;
    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override {
    // ImVec2 windowSize = ImGui::GetIO().DisplaySize;
    ScopedTimer totalTimeTimer("TOTAL TIME");
    {
      ScopedTimer clearTimer("CLEAR SCREEN TIMER");
      // Clear(olc::BLACK);
      ClearOptimized();
    }

    {
      ScopedTimer clearDepthBufferTimer("CLEAR DEPTH BUFFER");
      // for(int i = 0; i < ScreenWidth() * ScreenHeight(); i++)
      // RI.depthBuffer[i] = 0.0f;
      std::memset(RI.depthBuffer, 0,
                  ScreenHeight() * ScreenWidth() * sizeof(float));
    }

    trianglesRasteredCount = 0;
    // Variable aliases
    Vector3D &cameraPosition = player->camera.cameraPosition;
    const float &farPlane = player->camera.GetFacingPlanes().second;

    // Get the View Matrix after input
    Matrix4x4 viewMatrix = DoInputLoop(this, player);

    // Calculation loop
    for (const auto &mesh : allObjects.GetMeshList()) {
      if (mesh->doAutomaticRotation == true) {
        for (int i = 0; i < 3; i++)
          if (mesh->doAutomaticRotations[i] == true)
            mesh->rotationDegrees[i] +=
                fElapsedTime * 0.5f * mesh->rotationSpeeds[i];
      }
      Matrix4x4 scalingMatrix, rotationMatrix, translationMatrix;
      scalingMatrix =
          GetScalingMatrix(mesh->scalingOffsets[0], mesh->scalingOffsets[1],
                           mesh->scalingOffsets[2]);
      if (mesh->isStatic == true)
        rotationMatrix = GetCompoundRotationMatrix(
            ROT_TYPES::ROT_ZYX, mesh->rotationDegrees[0],
            mesh->rotationDegrees[1], mesh->rotationDegrees[2]);
      else // mesh->isStatic == false
        rotationMatrix =
            GetLookAtRotationMatrix(mesh->forwardVector, mesh->lookAtVector);

      translationMatrix = GetTranslationMatrix(mesh->translationOffsets[0],
                                               mesh->translationOffsets[1],
                                               mesh->translationOffsets[2]);
      Matrix4x4 tempMatrix =
          MultiplyMatrixMatrix(scalingMatrix, rotationMatrix);
      Matrix4x4 worldMatrix =
          MultiplyMatrixMatrix(tempMatrix, translationMatrix);
      for (const auto &triangle : mesh->GetTriangles()) {
        Vector3D normal;
        Triangle transformedTriangle;
        transformedTriangle = MultiplyTriangle(triangle, worldMatrix);
        normal = GetNormal(transformedTriangle);
        Triangle cameraTransformedTriangle;

        // Only draw triangles if the normal says its fits on screen
        Vector3D cameraRay =
            SubtractVector(transformedTriangle.points[0], cameraPosition);
        if (GetDotProduct(normal, cameraRay) < 0.0f &&
            GetDistanceBetweenPoints(player->camera.cameraPosition,
                                     transformedTriangle.points[0]) <=
                farPlane) {
          // materials phase
          cameraTransformedTriangle =
              MultiplyTriangle(transformedTriangle, viewMatrix);

          switch (mesh->GetMaterialType()) {
          case MATERIAL_TYPES::NONE:
            [[unlikely]];
            {
              float luminance = GetNoneMaterialLuminances(normal, allLights);
              cameraTransformedTriangle.color =
                  GetNoneMaterialColorCode(luminance);
              break;
            }

          case MATERIAL_TYPES::DIFFUSE: {
            // const olc::Pixel& tempColor = *(mesh->GetDiffuseColor());
            // olc::Pixel finalColor = mesh->doLighting ?
            // GetDiffuseMaterialColor(normal, tempColor, allLights) :
            // tempColor;
            cameraTransformedTriangle.color =
                (mesh->doLighting)
                    ? GetDiffuseMaterialColor(
                          normal, *(mesh->GetDiffuseColor()), allLights)
                    : *(mesh->GetDiffuseColor());
            break;
          }
          case MATERIAL_TYPES::TEXTURE:
          case MATERIAL_TYPES::TEXTURE_WITH_NORMAL:
          case MATERIAL_TYPES::COMPOSITE:
            break;
          }

          // View space clipping phase
          ScopedTimer viewSpaceClippingTimer("VIEW SPACE CLIPPING");
          DoViewSpaceClipping(this, player, trianglesToRaster, normalsToRaster,
                              cameraTransformedTriangle);
          if (SETTINGS_MAP[SETTINGS_ENUM::DO_SCREEN_SPACE_CLIPPING] == true) {
            for (const auto &triangleForScreenSpaceClipping :
                 trianglesToRaster) {
              ScopedTimer screenSpaceClippingTimer("SCREEN SPACE CLIPPING");
              DoScreenSpaceClipping(RI, triangleForScreenSpaceClipping,
                                    screenSpaceClippedTriangles);
            }
            for (int tri = 0; tri < screenSpaceClippedTriangles.size(); tri++) {
              DrawTriangleToScreen(RI, screenSpaceClippedTriangles[tri], normal,
                                   allLights, mesh->GetMaterialType(),
                                   mesh->GetTextureImage());
              if (mesh->doLighting == false)
                FillTriangleWithDepthBuffer(screenSpaceClippedTriangles[tri],
                                            RI);
            }
          } else if (SETTINGS_MAP[SETTINGS_ENUM::DO_SCREEN_SPACE_CLIPPING] ==
                     false) {
            ScopedTimer screenSpaceClippingTimer("DRAWING TRIANGLES");
            for (int triNoScreenSpaceClip = 0;
                 triNoScreenSpaceClip < trianglesToRaster.size();
                 triNoScreenSpaceClip++) {
              DrawTriangleToScreen(RI, trianglesToRaster[triNoScreenSpaceClip],
                                   normal, allLights, mesh->GetMaterialType(),
                                   mesh->GetTextureImage());
            }
          }
          if (SETTINGS_MAP[SETTINGS_ENUM::DRAW_NORMALS] == true) {
            DrawNormalsToScreen(RI, player->camera.GetCameraProjectionMatrix(),
                                normalsToRaster);
          }
          trianglesRasteredCount += trianglesToRaster.size();
          trianglesToRaster.clear();
          normalsToRaster.clear();
          screenSpaceClippedTriangles.clear();
        }
      }
      // Arifacts from solving bug number 2
      // PrintTrianglesToDisk(preClipTris,
      // ConcatenatePaths({GetPathFromResources(),
      // "TRIANGLES_PRE_SCREEN_SPACE_CLIPPING.txt"}));
      // PrintTrianglesToDisk(trianglesToRaster,
      // ConcatenatePaths({GetPathFromResources(),
      // "SCREEN_SPACE_CLIP_TRIS.txt"}));
    }

    if (SETTINGS_MAP[DO_DEBUG_MENU] == true) {
      u32string currentPosition = GetU32String(cameraPosition.ExtractInfo());
      u32string trianglesCount =
          GetU32String(to_string(trianglesRasteredCount));
      olc::vi2d mousePosition = this->GetMousePos();
      u32string mouseString = GetU32String(mousePosition.str());
      u32string facingString =
          GetU32String(player->camera.GetFacingVector().ExtractInfo());
      fontFreeSansBold->DrawString(U"Location: " + currentPosition, {10, 60},
                                   olc::WHITE);
      fontFreeSansBold->DrawString(U"Triangle Count: " + trianglesCount,
                                   {10, 120}, olc::WHITE);
      fontFreeSansBold->DrawString(U"Mouse Position: " + mouseString, {10, 180},
                                   olc::WHITE);
      fontFreeSansBold->DrawString(U"Facing: " + facingString, {10, 240},
                                   olc::WHITE);
    }

    // Draw Updated GUI Components
    manager.Update();
    manager.Draw();
    SetDrawTarget(m_GameLayer);
    // ImGui::ShowDemoWindow();
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
        std::string label = "Mesh " + std::to_string(meshIndex) + ": " + mesh->GetMeshName();
        if (ImGui::CollapsingHeader(label.c_str())) {
          ImGui::Text("Total Triangles: %d", mesh->GetTotalTriangles());
          ImGui::DragFloat3("Position", mesh->translationOffsets.data(), 0.1f);
          ImGui::DragFloat3("Rotation", mesh->rotationDegrees.data(), 0.1f);
          ImGui::DragFloat3("Scale", mesh->scalingOffsets.data(), 0.1f, 0.01f, 100.0f);
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
      // This is where screenshots are taken
      DoAuxiliaryInputLoop(this, allObjects, allLights);
      if (InputManager::KeyHeld(this, {NUM_9}))
        this->isRunning = false;
      return this->isRunning;
    }

    bool OnUserDestroy() override {
      this->isRunning = false;
      if (this->player != nullptr)
        delete this->player;
      if (this->fontFreeSans != nullptr)
        delete this->fontFreeSans;
      if (this->fontHackButtons != nullptr)
        delete this->fontHackButtons;
      if (this->fontFreeSansBold != nullptr)
        delete this->fontFreeSansBold;
      manager.DeleteAllControls();
      for (const auto &lightObject : allLights)
        if (lightObject != nullptr)
          delete lightObject;
      for (const auto &object : allObjects.GetMeshList())
        if (object != nullptr)
          delete object;

      return true;
    }

    void DrawUI(void) {
      ImGui::Checkbox("Do Debug Menu", &SETTINGS_MAP[DO_DEBUG_MENU]);
      ImGui::Checkbox("Draw Lines", &SETTINGS_MAP[DRAW_LINES]);
      ImGui::Checkbox("Draw Faces", &SETTINGS_MAP[DRAW_FACES]);
      ImGui::Checkbox("Visualize Clipping", &SETTINGS_MAP[VISUALIZE_CLIPPING]);
      ImGui::Checkbox("Do Screen Space Clipping",
                      &SETTINGS_MAP[DO_SCREEN_SPACE_CLIPPING]);
      ImGui::Checkbox("Do View Space Clipping",
                      &SETTINGS_MAP[DO_VIEW_SPACE_CLIPPING]);
      ImGui::Checkbox("Draw Normals", &SETTINGS_MAP[DRAW_NORMALS]);

      // This finishes the Dear ImGui and renders it to the screen

      pge_imgui.ImGui_ImplPGE_Render();
    }
    void DrawFileBrowser(bool *open, std::string &selected_path) {
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
    ~EngineReborn() {}
  };

  int main(int argc, char **argv) {
    // Initialize the pwd of the program
    PROGRAM_ROOT_DIRECTORY = GetExecutableDirectory(argv[0]);
    EngineReborn engine;
    if (engine.Construct(1920, 1080, 1, 1, false, false)) {
      engine.Start();
    } else
      cerr << "FATAL ERROR: Failed to create 3D engine window.\n";
    return 0;
  }
