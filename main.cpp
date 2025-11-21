#define OLC_PGE_APPLICATION
#define OLC_IMAGE_STB
#define OLC_PGEX_CUSTOMFONT
#define OLC_PGEX_FONT
#define OLC_PGEX_SPLASHSCREEN
#define OLC_PGEX_TTF
#define OLC_PGEX_MINIAUDIO

#include <iostream>
#include <string>
#include "globalVariables.h"
#include "essentialFunctions.h"
#include "miscFunctions.h"
#include "geometricPrimitives.h"
#include "matrixMathEssentials.h"
#include "normalMathEssentials.h"
#include "vectorMathEssentials.h"
#include "triangleMathEssentials.h"
#include "clippingRoutines.h"
#include "olcPixelGameEngine.h"
#include "olcPGEX_SplashScreen.h"
#include "guiEssentials.h"
#include "miscPrimitives.h"
#include "inputManager.h"
#include "audioManager.h"
#include "timerUtility.h"

#define DEFAULT_COLORS olc::BLUE, olc::GREEN, olc::RED

using namespace std;

unsigned long long totalFPS = 0;
unsigned long long totalTimes = 0;

class EngineReborn : public olc::PixelGameEngine
{
  //Splash screen. Initializing it causes it to occur
  olc::SplashScreen* sps = nullptr;
  float* depthBuffer = nullptr;

  //GUI related stuff 
  Manager manager;
  CheckBox* checkDrawLines = nullptr;
  CheckBox* checkDrawFaces = nullptr;
  CheckBox* checkDoDebugMenu = nullptr;
  CheckBox* checkDrawNormals = nullptr;
  CheckBox* checkVisualizeClipping = nullptr;
  CheckBox* checkDoScreenSpaceClipping = nullptr;
  CheckBox* checkDoViewSpaceClipping = nullptr;   
  CheckBox* checkShowOptionsMenu = nullptr;

  //Custom fonts
  olc::Font* fontFreeSans = nullptr;
  olc::Font* fontFreeSansBold = nullptr;
  olc::Font* fontHackButtons = nullptr;

  //Rendering instance used to efficiently pass information between functions and phases
  RenderingInstance RI;
  public:
    EngineReborn()
    {
      sAppName = "3D Viewer Reborn";
      #ifdef BUILD_RELEASE
        sps = new olc::SplashScreen();
      #endif
    }

  public:
    //Object variables
    MeshList allObjects;
    deque<Light*> allLights;
    float totalElapsedTime = 0.0f;
    Matrix4x4 zRotMat = GetIdentityMatrix();
    Matrix4x4 yRotMat = GetIdentityMatrix();
    Matrix4x4 xRotMat = GetIdentityMatrix();
    Player* player;

    //Consideration: Should this be another data structure?
    vector<Triangle> trianglesToRaster;
    vector<Vector3D> normalsToRaster;
    int trianglesRasteredCount = 0;
    //Artifact from bug number 2 hunting
    //vector<Triangle> preClipTris;
    
  bool OnUserCreate() override
  {
    //Initializing ASPECT_RATIO
    ASPECT_RATIO = (float)ScreenHeight() / (float)ScreenWidth();

    //Initializing fonts 
    olc::Font::init();

    InputManager::InitializeInputs(GetPathFromConfig({"input.yaml"}));

    string pathToScreenshots = GetPath({PROGRAM_ROOT_DIRECTORY, "..","..",}) + string("/screenshots");
    if(filesystem::exists(pathToScreenshots) == false)
    {
      cout << "Screenshots directory does not exist. Creating it now...\n";
      filesystem::create_directory(pathToScreenshots);
    }

    //Initialize single player instance
    player = new Player();
    player->camera.CalculateProjectionMatrix();
    player->camera.SetMovementSpeeds(10.0f, 10.0f, 150.0f);

    //Initialize missing texture sprite. Will crash if fails
    MISSING_TEXTURE_SPRITE = new olc::Sprite(GetPathFromResources({"textures","missingTexture.png"}));
    //Initialize RenderingInstance
    RI.InitializeRenderingInstance(this);

    //Initializing Custom fonts 
    fontFreeSans = new olc::Font(GetPathFromResources({"fonts", "TTF", "Hack-Regular.ttf"}), 50);
    fontFreeSansBold = new olc::Font(GetPathFromResources({"fonts", "TTF", "FreeSansBold.ttf"}), 50);
    fontHackButtons = new olc::Font(GetPathFromResources({"fonts", "TTF", "FreeSans.ttf"}), 25);

    //Initialize GUI elements
    checkDoDebugMenu = new CheckBox(this, &manager, fontHackButtons, "Do Debug Menu", {0,420}, DEFAULT_COLORS, 0.0f, {20,20}, SETTINGS_MAP[DO_DEBUG_MENU]);
    checkDrawLines = new CheckBox(this, &manager, fontHackButtons, "Draw Lines", {0,500}, DEFAULT_COLORS, 0.0f, {20,20}, SETTINGS_MAP[DRAW_LINES]);
    checkDrawFaces = new CheckBox(this, &manager, fontHackButtons, "Draw Faces", {0, 580}, DEFAULT_COLORS, 0.0f, {20,20}, SETTINGS_MAP[DRAW_FACES]);
    checkVisualizeClipping = new CheckBox(this, &manager, fontHackButtons, "Visualize Clipping", {0, 660}, DEFAULT_COLORS, 0.0f, {20,20}, SETTINGS_MAP[VISUALIZE_CLIPPING]);
    checkDoScreenSpaceClipping = new CheckBox(this, &manager, fontHackButtons, "Do Screen Space Clipping", {0, 740}, DEFAULT_COLORS, 0.0f, {20,20}, SETTINGS_MAP[DO_SCREEN_SPACE_CLIPPING]);
    checkDoViewSpaceClipping = new CheckBox(this, &manager, fontHackButtons, "Do View Space Clipping", {0,820}, DEFAULT_COLORS, 0.0f, {20,20}, SETTINGS_MAP[DO_VIEW_SPACE_CLIPPING]);
    checkDrawNormals = new CheckBox(this, &manager, fontHackButtons, "Draw Normals", {0,900}, DEFAULT_COLORS, 0.0f, {20,20}, SETTINGS_MAP[DRAW_NORMALS], true);
    checkShowOptionsMenu = new CheckBox(this, &manager, fontHackButtons, "Show Options", {0,980}, DEFAULT_COLORS, 0.0f,{20,20}, true);
  
    SetInitialObjects(this, allObjects, allLights, NUM_1);
    #ifdef BUILD_DEBUG
      PrintAllPrimitiveSizes();
    #endif
    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override
  {
    ScopedTimer totalTimeTimer("TOTAL TIME");
    {
      ScopedTimer clearTimer("CLEAR SCREEN TIMER");
      //Clear(olc::BLACK);
      ClearOptimized();
    }

    {
      ScopedTimer clearDepthBufferTimer("CLEAR DEPTH BUFFER");
      //for(int i = 0; i < ScreenWidth() * ScreenHeight(); i++)
        //RI.depthBuffer[i] = 0.0f;
      std::memset(RI.depthBuffer, 0, ScreenHeight() * ScreenWidth() * sizeof(float));
    }

      trianglesToRaster.clear();
      normalsToRaster.clear();
      trianglesRasteredCount = 0;
    //Variable aliases
    Vector3D& cameraPosition = player->camera.cameraPosition;
    const float& farPlane = player->camera.GetFacingPlanes().second;

    //Get the View Matrix after input
    Matrix4x4 viewMatrix = DoInputLoop(this, player);
    
    //Calculation loop
    for(const auto& mesh : allObjects.GetMeshList())
    {
      if(mesh->doAutomaticRotation == true)
      {
        for(int i = 0; i < 3; i++)
          if(mesh->doAutomaticRotations[i] == true)
            mesh->rotationDegrees[i] += fElapsedTime * 0.5f * mesh->rotationSpeeds[i];
      }
      //TODO: implement the WORLD MATRIX calculations 
      Matrix4x4 scalingMatrix, rotationMatrix, translationMatrix;
      scalingMatrix = GetScalingMatrix(mesh->scalingOffsets[0], mesh->scalingOffsets[1], mesh->scalingOffsets[2]);
      if(mesh->isStatic == true)
        rotationMatrix = GetCompoundRotationMatrix(ROT_TYPES::ROT_ZYX, mesh->rotationDegrees[0], 
                                                 mesh->rotationDegrees[1], mesh->rotationDegrees[2]);
      else //mesh->isStatic == false
        rotationMatrix = GetLookAtRotationMatrix(mesh->forwardVector, mesh->lookAtVector);

      translationMatrix = GetTranslationMatrix(mesh->translationOffsets[0], mesh->translationOffsets[1],
                                               mesh->translationOffsets[2]);
      Matrix4x4 tempMatrix = MultiplyMatrixMatrix(scalingMatrix, rotationMatrix);
      Matrix4x4 worldMatrix = MultiplyMatrixMatrix(tempMatrix, translationMatrix);
      for(const auto& triangle : mesh->triangles)
      {
        Vector3D normal;
        Triangle transformedTriangle;
        transformedTriangle = MultiplyTriangle(triangle, worldMatrix);
        normal = GetNormal(transformedTriangle);
        Triangle cameraTransformedTriangle;

        //Only draw triangles if the normal says its fits on screen
        Vector3D cameraRay = SubtractVector(transformedTriangle.points[0], cameraPosition);
        if(GetDotProduct(normal, cameraRay) < 0.0f &&
          GetDistanceBetweenPoints(player->camera.cameraPosition, normal) <= farPlane)
        {
          //materials phase
          cameraTransformedTriangle = MultiplyTriangle(transformedTriangle, viewMatrix);

          //TO-DO: Switch this to a switch case
          if(mesh->GetMaterialType() == MATERIAL_TYPES::NONE)
          {
            float luminance = GetNoneMaterialLuminances(normal, allLights);
            cameraTransformedTriangle.color = GetNoneMaterialColorCode(luminance);
          }
          
          else if(mesh->GetMaterialType() == MATERIAL_TYPES::DIFFUSE)
          {
            const olc::Pixel& tempColor = *(mesh->GetDiffuseColor());
            olc::Pixel finalColor = GetDiffuseMaterialColor(normal, tempColor, allLights);
            cameraTransformedTriangle.color = finalColor;
          }

          //View space clipping phase          
          ScopedTimer viewSpaceClippingTimer("VIEW SPACE CLIPPING");
          DoViewSpaceClipping(this, player, trianglesToRaster, normalsToRaster, cameraTransformedTriangle);
        }
      }
      //Arifacts from solving bug number 2
      //PrintTrianglesToDisk(preClipTris, ConcatenatePaths({GetPathFromResources(), "TRIANGLES_PRE_SCREEN_SPACE_CLIPPING.txt"}));
      //PrintTrianglesToDisk(trianglesToRaster, ConcatenatePaths({GetPathFromResources(), "SCREEN_SPACE_CLIP_TRIS.txt"}));
      
      //Sorting section
      //They are sorted according to distance from camera, furthest objects are drawn first
      //SortTriangles(trianglesToRaster);
      //Not needed anymore with texturing since a depth buffer is used. However, regular drawing
      //routines don't use it. They will have to be reimplemented or edited

      //Screen edges clipping and rasterization section
      //Rasterizing normals(if settings allow it)
      
        {
            ScopedTimer screenSpaceClippingTimer("SCREEN SPACE CLIPPING");
            DoScreenSpaceClipping(RI, trianglesToRaster, normalsToRaster, mesh);
        }
      trianglesRasteredCount += trianglesToRaster.size();
      trianglesToRaster.clear();
      normalsToRaster.clear();
    }

    if(SETTINGS_MAP[DO_DEBUG_MENU] == true)
    {
      u32string currentPosition = GetU32String(cameraPosition.ExtractInfo());
      u32string trianglesCount = GetU32String(to_string(trianglesRasteredCount));
      olc::vi2d mousePosition = this->GetMousePos();
      u32string mouseString = GetU32String(mousePosition.str());
      u32string facingString = GetU32String(player->camera.GetFacingVector().ExtractInfo());
      fontFreeSansBold->DrawString(U"Location: " + currentPosition, {10,60}, olc::WHITE);
      fontFreeSansBold->DrawString(U"Triangle Count: " + trianglesCount, {10,120}, olc::WHITE);
      fontFreeSansBold->DrawString(U"Mouse Position: " + mouseString, {10,180}, olc::WHITE);
      fontFreeSansBold->DrawString(U"Facing: " + facingString, {10, 240}, olc::WHITE);
    }

    SETTINGS_MAP[DO_DEBUG_MENU] = checkDoDebugMenu->state;
    SETTINGS_MAP[DRAW_LINES] = checkDrawLines->state;
    SETTINGS_MAP[DRAW_FACES] = checkDrawFaces->state;
    SETTINGS_MAP[VISUALIZE_CLIPPING] = checkVisualizeClipping->state;
    SETTINGS_MAP[DO_SCREEN_SPACE_CLIPPING] = checkDoScreenSpaceClipping->state;
    SETTINGS_MAP[DO_VIEW_SPACE_CLIPPING] = checkDoViewSpaceClipping->state;
    SETTINGS_MAP[DRAW_NORMALS] = checkDrawNormals->state;

    bool temp = checkShowOptionsMenu->state;
    //There must be something about Stupid void* that I dont understand
    if(temp == true)
    {
      //manager.ChangeEnabledStatesExcept(checkShowOptionsMenu, true);
      checkDoDebugMenu->isEnabled = true;
      checkDrawLines->isEnabled = true;
      checkDrawFaces->isEnabled = true;
      checkVisualizeClipping->isEnabled = true;
      checkDoScreenSpaceClipping->isEnabled = true;
      checkDoViewSpaceClipping->isEnabled = true;
      checkDrawNormals->isEnabled = true;
    }

    else if(temp == false)
    {
      //manager.ChangeEnabledStatesExcept(checkShowOptionsMenu, false);
      checkDoDebugMenu->isEnabled = false;
      checkDrawLines->isEnabled = false;
      checkDrawFaces->isEnabled = false;
      checkVisualizeClipping->isEnabled = false;
      checkDoScreenSpaceClipping->isEnabled = false;
      checkDoViewSpaceClipping->isEnabled = false;
      checkDrawNormals->isEnabled = false;
    }
    //Draw Updated GUI Components
    manager.Update();
    manager.Draw();

    //This is where screenshots are taken
    DoAuxiliaryInputLoop(this, allObjects, allLights);
    return true;
  }
};

int main(int argc, char** argv)
{
  //Initialize the pwd of the program
  PROGRAM_ROOT_DIRECTORY = GetExecutableDirectory(argv[0]);
  EngineReborn engine;
  if(engine.Construct(1920, 1080, 1, 1, false, false))
  {
    engine.Start();
  }
  else
    cerr << "FATAL ERROR: Failed to create 3D engine window.\n";
}
