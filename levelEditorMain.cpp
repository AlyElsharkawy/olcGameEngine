#define OLC_PGE_APPLICATION
#define OLC_PGEX_DEAR_IMGUI_IMPLEMENTATION
#include "clippingRoutines.h"
#include "drawingRoutines.h"
#include "essentialFunctions.h"
#include "globalVariables.h"
#include "imgui_impl_pge.h"
#include "levelEditor.h"
#include "matrixMathEssentials.h"
#include "miscFunctions.h"
#include "miscPrimitives.h"
#include "normalMathEssentials.h"
#include "olcPixelGameEngine.h"
#include "triangleMathEssentials.h"
#include "vectorMathEssentials.h"
#include <cstring>

class LevelEditorApp : public olc::PixelGameEngine {
public:
  LevelEditorApp() : pge_imgui(false) { sAppName = "Level Editor"; }

  LevelEditor editor;
  MeshList meshList;
  olc::imgui::PGE_ImGUI pge_imgui;
  int m_GameLayer;

  // Rendering components
  RenderingInstance RI;
  Player *player = nullptr;
  deque<Light *> allLights;
  olc::Decal *viewportDecal = nullptr;

  // Rendering buffers
  vector<Triangle> trianglesToRaster;
  vector<pair<Vector3D, Vector3D>> normalsToRaster;
  deque<Triangle> screenSpaceClippedTriangles;

  bool OnUserCreate() override {
    // Initialize Aspect Ratio
    ASPECT_RATIO = (float)ScreenHeight() / (float)ScreenWidth();

    // Initialize Player/Camera
    player = new Player();
    player->camera.CalculateProjectionMatrix();
    player->camera.SetMovementSpeeds(10.0f, 10.0f, 150.0f);

    // Initialize Inputs
    InputManager::InitializeInputs(GetPathFromConfig({"input.yaml"}));

    // Initialize Rendering Instance
    std::cout << "Initializing Rendering Instance..." << std::endl;
    RI.InitializeRenderingInstance(this);

    // Add a default light
    allLights.push_back(new Light(LIGHT_TYPES::LAMP_SUN, {0, -1, 0}, {0, 10, 0},
                                  "Default Light"));

    // Setup Layer
    std::cout << "Creating Layer..." << std::endl;
    m_GameLayer = CreateLayer();
    EnableLayer(m_GameLayer,
                false); // Disable rendering to main screen directly
    SetLayerCustomRenderFunction(
        0, std::bind(&olc::imgui::PGE_ImGUI::ImGui_ImplPGE_Render, &pge_imgui));

    std::cout << "Creating Viewport Decal..." << std::endl;
    SetDrawTarget((uint8_t)m_GameLayer);
    olc::Sprite *layerSprite = GetDrawTarget();
    if (layerSprite == nullptr) {
      std::cout << "Error: Failed to get draw target for layer " << m_GameLayer
                << std::endl;
      return false;
    }
    viewportDecal = new olc::Decal(layerSprite);
    SetDrawTarget(nullptr);

    std::cout << "Initializing ImGui Font..." << std::endl;
    // Initialize ImGui explicitly since we deferred it
    pge_imgui.ImGui_ImplPGE_Init();

    //1. Get the IO structure
    ImGuiIO &io = ImGui::GetIO();

    // 2. Load the font (usually a .ttf or .otf file)
    // Passing '18.0f' sets the font size
    std::string fontPath =
        GetPathFromResources({"fonts", "TTF", "Hack-Regular.ttf"});
    ImFont *myFont = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 20);

    // 3. (Optional) Set as default
    io.FontDefault = myFont;
    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override {
    // Input Handling
    Matrix4x4 viewMatrix = DoInputLoop(this, player);

    // Rendering
    SetDrawTarget((uint8_t)m_GameLayer);
    Clear(olc::BLACK);

    // Clear Depth Buffer
    if (RI.depthBuffer)
      std::memset(RI.depthBuffer, 0,
                  ScreenHeight() * ScreenWidth() * sizeof(float));
    else
      std::cout << "Error: Depth buffer is null!" << std::endl;

    Vector3D &cameraPosition = player->camera.cameraPosition;
    const float &farPlane = player->camera.GetFacingPlanes().second;

    for (const auto &mesh : meshList.GetMeshList()) {
      Matrix4x4 scalingMatrix =
          GetScalingMatrix(mesh->scalingOffsets[0], mesh->scalingOffsets[1],
                           mesh->scalingOffsets[2]);
      Matrix4x4 rotationMatrix = GetCompoundRotationMatrix(
          ROT_TYPES::ROT_ZYX, mesh->rotationDegrees[0],
          mesh->rotationDegrees[1], mesh->rotationDegrees[2]);
      Matrix4x4 translationMatrix = GetTranslationMatrix(
          mesh->translationOffsets[0], mesh->translationOffsets[1],
          mesh->translationOffsets[2]);

      Matrix4x4 worldMatrix = MultiplyMatrixMatrix(
          MultiplyMatrixMatrix(scalingMatrix, rotationMatrix),
          translationMatrix);

      for (const auto &triangle : mesh->GetTriangles()) {
        Triangle transformedTriangle = MultiplyTriangle(triangle, worldMatrix);
        Vector3D normal = GetNormal(transformedTriangle);

        Vector3D cameraRay =
            SubtractVector(transformedTriangle.points[0], cameraPosition);

        if (GetDotProduct(normal, cameraRay) < 0.0f &&
            GetDistanceBetweenPoints(
                cameraPosition, transformedTriangle.points[0]) <= farPlane) {
          Triangle cameraTransformedTriangle =
              MultiplyTriangle(transformedTriangle, viewMatrix);

          switch (mesh->GetMaterialType()) {
          case MATERIAL_TYPES::NONE: {
            float luminance = GetNoneMaterialLuminances(normal, allLights);
            cameraTransformedTriangle.color =
                GetNoneMaterialColorCode(luminance);
            break;
          }
          case MATERIAL_TYPES::DIFFUSE: {
            cameraTransformedTriangle.color =
                (mesh->doLighting)
                    ? GetDiffuseMaterialColor(
                          normal, *(mesh->GetDiffuseColor()), allLights)
                    : *(mesh->GetDiffuseColor());
            break;
          }
          case MATERIAL_TYPES::TEXTURE:
          case MATERIAL_TYPES::COMPOSITE:
            break;
          }

          // Clipping and Drawing
          DoViewSpaceClipping(this, player, trianglesToRaster, normalsToRaster,
                              cameraTransformedTriangle);

          for (const auto &tri : trianglesToRaster) {
            DoScreenSpaceClipping(RI, tri, screenSpaceClippedTriangles);
          }

          for (const auto &clippedTri : screenSpaceClippedTriangles) {
            DrawTriangleToScreen(RI, clippedTri, normal, allLights,
                                 mesh->GetMaterialType(),
                                 mesh->GetTextureImage());
            if (!mesh->doLighting)
              FillTriangleWithDepthBuffer(clippedTri, RI);
          }
          
          if(SETTINGS_MAP[SETTINGS_ENUM::DRAW_NORMALS] == true)
          {
            DrawNormalsToScreen(RI, player->camera.GetCameraProjectionMatrix(), normalsToRaster); 
          }

          trianglesToRaster.clear();
          normalsToRaster.clear();
          screenSpaceClippedTriangles.clear();
        }
      }
    }

    // Draw UI
    if (viewportDecal)
      viewportDecal->Update();
    SetDrawTarget(nullptr);
    Clear(olc::BLACK);
    editor.DrawUI(fElapsedTime, meshList,
                  viewportDecal ? viewportDecal->id : -1);

    return true;
  }

  bool OnUserDestroy() override {
    if (player)
      delete player;
    if (viewportDecal)
      delete viewportDecal;
    for (auto light : allLights)
      delete light;
    for (auto mesh : meshList.GetMeshList())
      delete mesh;
    return true;
  }
};

int main(int argc, char **argv) {
  PROGRAM_ROOT_DIRECTORY = GetExecutableDirectory(argv[0]);
  LevelEditorApp demo;
  if (demo.Construct(1920, 1090, 1, 1))
    demo.Start();
  return 0;
}
