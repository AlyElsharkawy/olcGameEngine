#define OLC_PGE_APPLICATION
#define OLC_PGEX_DEAR_IMGUI_IMPLEMENTATION
#include "olcPixelGameEngine.h"
#include "imgui_impl_pge.h"
#include "levelEditor.h"

class LevelEditorApp : public olc::PixelGameEngine
{
public:
    LevelEditorApp() : pge_imgui(false)
    {
        sAppName = "Level Editor";
    }

    LevelEditor editor;
    MeshList meshList;
    olc::imgui::PGE_ImGUI pge_imgui;
    int m_GameLayer;

    bool OnUserCreate() override
    {
        m_GameLayer = CreateLayer();
        EnableLayer(m_GameLayer, true);
        SetLayerCustomRenderFunction(0, std::bind(&olc::imgui::PGE_ImGUI::ImGui_ImplPGE_Render, &pge_imgui));
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        SetDrawTarget((uint8_t)m_GameLayer);
        Clear(olc::BLACK);

        // Draw UI
        editor.DrawUI(fElapsedTime, meshList);

        return true;
    }
};

int main()
{
    LevelEditorApp demo;
    if (demo.Construct(1280, 720, 2, 2))
        demo.Start();
    return 0;
}
