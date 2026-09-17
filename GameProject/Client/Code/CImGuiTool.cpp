#include "pch.h"
#include "CImGuiTool.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"

HRESULT CImGuiTool::Ready(HWND hWnd, LPDIRECT3DDEVICE9 pDevice)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.f;
        style.Colors[ImGuiCol_WindowBg].w = 1.f;
    }

    if (!ImGui_ImplWin32_Init(hWnd))  return E_FAIL;
    if (!ImGui_ImplDX9_Init(pDevice)) return E_FAIL;

    return S_OK;
}

void CImGuiTool::BeginFrame()
{
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void CImGuiTool::EndFrame()
{
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void CImGuiTool::Release()
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void CImGuiTool::OnLostDevice() { ImGui_ImplDX9_InvalidateDeviceObjects(); }
void CImGuiTool::OnResetDevice() { ImGui_ImplDX9_CreateDeviceObjects(); }