#define OLC_PGE_APPLICATION
#define OLC_IMAGE_STB
#define PGE_GFX_OPENGL33
#define OLC_PGEX_DEAR_IMGUI_IMPLEMENTATION
#include <iostream>
#include "olcPixelGameEngine.h"
#include "imgui_impl_pge.h"

using namespace std;

class Example : public olc::PixelGameEngine
{
    olc::imgui::PGE_ImGUI pge_imgui;
    int m_GameLayer;

public:
    //PGE_ImGui can automatically call the SetLayerCustomRenderFunction by passing
    //true into the constructor.  false is the default value.
    Example() : pge_imgui(false)
    {
        sAppName = "Test Application";
    }

public:
    bool OnUserCreate() override
    {
        //Create a new Layer which will be used for the game
        m_GameLayer = CreateLayer();
        //The layer is not enabled by default,  so we need to enable it
        EnableLayer(m_GameLayer, true);

        //Set a custom render function on layer 0.  Since DrawUI is a member of
        //our class, we need to use std::bind
        //If the pge_imgui was constructed with _register_handler = true, this line is not needed
        SetLayerCustomRenderFunction(0, std::bind(&Example::DrawUI, this));

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        //Change the Draw Target to not be Layer 0
        SetDrawTarget((uint8_t)m_GameLayer);
        //Game Drawing code here

        //Create and react to your UI here, it will be drawn during the layer draw function
        //ImGui::ShowDemoWindow();
        //ImGui::Text("Hello, World!");
        /*vector<char> buf(50);
        float f;
        ImGui::Text("Hello, world %d", 123);
        if (ImGui::Button("Save"))
          cout << "hambola\n";
        ImGui::InputText("string", buf.data(), buf.size() );
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);*/   
    // Main window
     bool show_context_window = false;
    char file_path[256] = "";  // Buffer for file path
        ImGui::Begin("Main Window");

        // Button to open context window
        if (ImGui::Button("Open File Dialog"))
        {
            show_context_window = true;
        }
        
        ImGui::End();

        // Context window to input location
        if (show_context_window)
        {
            ImGui::Begin("Specify Location", &show_context_window);
            ImGui::InputText("File Path", file_path, IM_ARRAYSIZE(file_path));
            if (ImGui::Button("Save Location"))
            {
                std::cout << "Saved Path: " << file_path << std::endl;
                show_context_window = false;  // Close context window after saving location
            }
            ImGui::End();
        }

        return true;
    }

    void DrawUI(void) {
        //This finishes the Dear ImGui and renders it to the screen
        pge_imgui.ImGui_ImplPGE_Render();
    }
};

int main() {
    Example demo;
    if (demo.Construct(1920, 1080, 1, 1))
        demo.Start();

    return 0;
}
