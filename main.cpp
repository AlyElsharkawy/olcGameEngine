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
  CheckBox* checkDoPerformanceClearning = nullptr;
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
      if(SETTINGS_MAP[DO_SPLASH_SCREEN] == true)
      {
        sps = new olc::SplashScreen(); 
      }

      if(sps != nullptr && SETTINGS_MAP[DO_SPLASH_SCREEN] == true)
      {
        delete sps;
      }
    }

  public:
    //Object variables
    MeshList allObjects;
    deque<Light> allLights;
    float totalElapsedTime = 0.0f;
    Matrix4x4 zRotMat = GetIdentityMatrix();
    Matrix4x4 yRotMat = GetIdentityMatrix();
    Matrix4x4 xRotMat = GetIdentityMatrix();
    Player* player;

    //Camera variables
    float fTheta = 0.0f;

    //Consideration: Should this be another data structure?
    vector<Triangle> trianglesToRaster;
    vector<Vector3D> normalsToRaster;
    
  bool OnUserCreate() override
  {
    //Initializing ASPECT_RATIO
    ASPECT_RATIO = (float)ScreenHeight() / (float)ScreenWidth();

    //Initializing fonts 
    olc::Font::init();

    if(!filesystem::exists(GetPathFromConfig() + "/input.yaml"))
    {
      cout << "inputs.yaml does not exist. Creating it now...\n";
      WriteInitialInputs();
    }

    //Initialize input maps for adjustable inputs during load and run time 
    InitializeInputMaps();

    bool loadedInputs = InitializeInputs(this, GetPathFromConfig({"input.yaml"}));
    if(loadedInputs == false)
      return false;

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
    //Initalize RenderingInstance
    RI.InitializeRenderingInstance(this);
    RI.SetProjectionMatrix(player->camera.GetCameraProjectionMatrix());

    //Initializing Custom fonts 
    fontFreeSans = new olc::Font(GetPathFromResources({"fonts", "TTF", "Hack-Regular.ttf"}), 50);
    fontFreeSansBold = new olc::Font(GetPathFromResources({"fonts", "TTF", "FreeSansBold.ttf"}), 50);
    fontHackButtons = new olc::Font(GetPathFromResources({"fonts", "TTF", "FreeSans.ttf"}), 25);

    //Initialize GUI elements
    checkDoDebugMenu = new CheckBox(this, &manager, fontHackButtons, "Do Debug Menu", {0,420}, olc::BLUE, 0.0f, {20,20}, SETTINGS_MAP[DO_DEBUG_MENU]);
    checkDrawLines = new CheckBox(this, &manager, fontHackButtons, "Draw Lines", {0,500}, olc::BLUE, 0.0f, {20,20}, SETTINGS_MAP[DRAW_LINES]);
    checkDrawFaces = new CheckBox(this, &manager, fontHackButtons, "Draw Faces", {0, 580}, olc::BLUE, 0.0f, {20,20}, SETTINGS_MAP[DRAW_FACES]);
    checkVisualizeClipping = new CheckBox(this, &manager, fontHackButtons, "Visualize Clipping", {0, 660}, olc::BLUE, 0.0f, {20,20}, SETTINGS_MAP[VISUALIZE_CLIPPING]);
    checkDoScreenSpaceClipping = new CheckBox(this, &manager, fontHackButtons, "Do Screen Space Clipping", {0, 740}, olc::BLUE, 0.0f, {20,20}, SETTINGS_MAP[DO_SCREEN_SPACE_CLIPPING]);
    checkDoViewSpaceClipping = new CheckBox(this, &manager, fontHackButtons, "Do View Space Clipping", {0,820}, olc::BLUE, 0.0f, {20,20}, SETTINGS_MAP[DO_VIEW_SPACE_CLIPPING]);
    checkDoPerformanceClearning = new CheckBox(this, &manager,fontHackButtons, "Do Automatic Rotation", {0,900}, olc::BLUE, 0.0f, {20,20}, SETTINGS_MAP[DO_PERFORMANCE_CLEARING], true);
    checkShowOptionsMenu = new CheckBox(this, &manager, fontHackButtons, "Show Options", {0,980}, olc::BLUE, 0.0f,{20,20}, true);

    /*
      * UPDATE: As of 9/9/2024, mesh objects are now heap allocated!
      * Note: Memory management issues are painful
      * LIGHT SYNTAX
      * Light mainLamp = new Light(LIGHT_TYPE, LOCATION, COLOR, INTENSITY)
      * Location is a olc::vf2d
      * Color is a olc::Pixel
      * INTENSITY is a float from 0 to 1;
      *
      * Then push it back to the allLights object. The effect of lighting on a triangle/mesh will be
      * the average of all the lamps combined
      */
      
      /*
       *  Note: The GetPath family of functions take any number of inputs in a pair of braces {}
       *  GetPathFromResources() will return a path object pointing to the directories and files in
       *  the functions parameters. For example:
       *  GetPathFromResources({"folder1", "folder2", "file.txt"});
       *  example->SetTextureImage(GetPathFromResources("textures", "tex1.png"));
       *  example->SetDiffuseColor({255.0f, 128.0f, 0.0f}); this is an olc::Pixel()
       *
       *  In the above example, only the latest method will take effect. Thus, the mesh will have a
       *  diffuse color. The texture image will be deleted.
       *
       *  example->SetScalingOffsets(2.0f,2.0f, 2.0f)'
       *  example->SetTranslationOffsets(50.0f,50.f, 50.0f);
       *  example->doAutomaticRotation = true;
       *  
       *  example->isStatic = true //true is default
       *  if its not static, then you can set a lookAtVector parameter.
       *  The mesh will then be facing parallel to that vector.
       *  example->lookAtVector = {x,y,z,w}; Note: W is by default one
      *   example->doXRotation and example.doYRotation and example.doZRotation
      *   These are all bools that can be set to true and false.
      *   The mesh will only rotate if example->doAutomaticRotation is also set to true
      *   All of the above are defaulted to false
       * */

      /*
      * MESH OBJECT SYNTAX
      * First, you simply create the object 
      * Mesh example;
      * Then you can use its methods to modify it 
      * Load a .obj file: example->LoadFromOBJFile(GetPathFromResources("objectFiles", "example.obj"));
      * Then you can assign either a color or texture. If none are specified, then the mesh will be
      * shaded in black and white
      */

    Light mainLamp(LIGHT_TYPES::LAMP_SUN, {0.0f,1.0f,-1.0f}, {255, 255, 255}, 0.8f);
    allLights.push_back(mainLamp);
    //Light testLamp(LIGHT_TYPES::LAMP_SUN, {0.0f,1.0f,-1.0f}, {253, 184, 19}, 0.5f);
    //allLights.push_back(testLamp);

    //Initialize hard coded meshes
    Mesh* lightObj = new Mesh();
    lightObj->LoadFromOBJFile(GetPathFromResources({"objectFiles", "Primitives", "GoodCube.obj"}), true);
    lightObj->SetTranslationOffsets(0.0f,0.0f, 20.0f);
    lightObj->SetRotationSpeeds(1.0f, 1.0f, 1.0f);
    lightObj->doAutomaticRotation = false;
    //lightObj->SetDiffuseColor(210, 4, 45, 255);
    
    lightObj->SetTextureImage(GetPathFromResources({"textures", "stoneBrickWall.png"}));
    lightObj->PrintTextureInformation();
    lightObj->lookAtVector = mainLamp.GetDirection();
    lightObj->isStatic = true;
    
    /*Mesh mountainsObj;
    mountainsObj.LoadFromOBJFile(GetPathFromResources({"objectFiles", "Primitives", "mountains.obj"}), false);
    mountainsObj.SetScalingOffsets(0.25f, 0.25f, 0.25f);*/
    //allObjects.AppendMesh(mountainsObj);
    
    allObjects.AppendMesh(lightObj);
    allObjects.UpdateTotalCounts();
    cout << "Triangles in list: " << allObjects.GetTotalTriangles() << '\n';

    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override
  {
    if(SETTINGS_MAP[DO_PERFORMANCE_CLEARING] == true)
     ClearScreenPerformance(this, trianglesToRaster);
    else
     Clear(olc::BLACK);
    fTheta += fElapsedTime;
    
    //Variable aliases
    Vector3D& cameraPosition = player->camera.cameraPosition;
    const float& farPlane = player->camera.GetFacingPlanes().second;

    //Get the View Matrix after input
    Matrix4x4 viewMatrix = DoInputLoop(this, player);
    //Calculation loop
    Mesh* currentMesh;
    for(auto& mesh : allObjects.GetMeshList())
    {
      currentMesh = mesh;
      for(auto& triangle : mesh->triangles)
      {
        if(mesh->doAutomaticRotation == true)
        {
          for(int i = 0; i < 3; i++)
            mesh->rotationDegrees[1] += fElapsedTime * 0.1f;
        }

        Vector3D normal;

        Triangle scaledTriangle = ScaleTriangle(triangle, mesh->scalingOffsets[0], mesh->scalingOffsets[1], mesh->scalingOffsets[2]);
        Triangle rotatedTriangle;
        
        if(mesh->isStatic == false)
        {
          Matrix4x4 lookAtRotMatrix = GetLookAtRotationMatrix(mesh->forwardVector, mesh->lookAtVector);
          rotatedTriangle = MultiplyTriangle(scaledTriangle, lookAtRotMatrix);
        }

        if(mesh->isStatic == true)
        {
          Matrix4x4 rotationMatrix = GetCompoundRotationMatrix(ROT_TYPES::ROT_ZYX, mesh->rotationDegrees[0], mesh->rotationDegrees[1], mesh->rotationDegrees[2]);
          rotatedTriangle = MultiplyTriangle(scaledTriangle, rotationMatrix);
        }

        Triangle translatedTriangle = TranslateTriangle(rotatedTriangle, mesh->translationOffsets[0], mesh->translationOffsets[1], mesh->translationOffsets[2]);
        normal = GetNormal(translatedTriangle);

        Triangle cameraTransformedTriangle;

        //Only draw triangles if the normal says its fits on screen
        Vector3D cameraRay = SubtractVector(translatedTriangle.points[0], cameraPosition);
        if(GetDotProduct(normal, cameraRay) < 0.0f &&
          GetDistanceBetweenPoints(player->camera.cameraPosition, normal) <= farPlane)
        {
          //materials phase
          cameraTransformedTriangle = MultiplyTriangle(translatedTriangle, viewMatrix);

          //TO-DO: Switch this to a switch case
          if(mesh->GetMaterialType() == MATERIAL_TYPES::NONE)
          {
            float luminance = GetNoneMaterialLuminances(normal, allLights);
            cameraTransformedTriangle.color = GetNoneMaterialColorCode(luminance);
          }
          
          else if(mesh->GetMaterialType() == MATERIAL_TYPES::DIFFUSE)
          {
            olc::Pixel tempColor = *(mesh->GetDiffuseColor());
            olc::Pixel finalColor = GetDiffuseMaterialColor(normal, tempColor, allLights);
            cameraTransformedTriangle.color = finalColor;
          }

          //View space clipping phase
          DoViewSpaceClipping(this, player, trianglesToRaster, normalsToRaster, cameraTransformedTriangle);
        }
      }
      
      //Sorting section
      //They are sorted according to distance from camera, furthest objects are drawn first
      //SortTriangles(trianglesToRaster);
      //Not needed anymore with texturing since a depth buffer is used. However, regular drawing
      //routines don't use it. They will have to be reimplemented or edited

      //Screen edges clipping and rasterization section
      //Rasterizing normals(if settings allow it)
      //RasterizeNormal(this, cameraTransformedTriangle, player->camera.GetCameraProjectionMatrix(), normal);
      DoScreenSpaceClipping(RI, trianglesToRaster, normalsToRaster, *mesh);
    } 
    if(SETTINGS_MAP[DO_DEBUG_MENU] == true)
    {
      u32string currentPosition = GetU32String(cameraPosition.ExtractInfo());
      u32string trianglesCount = GetU32String(to_string(trianglesToRaster.size()));
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
    currentMesh->doAutomaticRotation = checkDoPerformanceClearning->state;

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
      checkDoPerformanceClearning->isEnabled = true;
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
      checkDoPerformanceClearning->isEnabled = false;
    }
    //Draw Updated GUI Components
    manager.Update();
    manager.Draw();

    trianglesToRaster.clear();
    normalsToRaster.clear();
    for(int i = 0; i < ScreenWidth() * ScreenHeight(); i++)
      RI.depthBuffer[i] = 0.0f;
    //This is where screenshots are taken
    DoAuxilliaryInputLoop(this);

    return true;
  }
};

int main(int argc, char** argv)
{
  //Initialize the pwd of the program
  PROGRAM_ROOT_DIRECTORY = GetExecutableDirectory(argv[0]);
  EngineReborn engine;
  if(engine.Construct(1920, 1080, 1, 1))
  {
    engine.Start();
  }
  else
    cerr << "FATAL ERROR: Failed to create 3D engine window.\n";
}
