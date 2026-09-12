#pragma once
#include <d3d9.h>
#pragma push_macro("new")
#undef new
#include "imgui.h"
#pragma pop_macro("new")

class CImGuiTool
{
public:
    static HRESULT Ready(HWND hWnd, LPDIRECT3DDEVICE9 pDevice);
    static void    BeginFrame();
    static void    EndFrame();
    static void    Release();

    static void    OnLostDevice();
    static void    OnResetDevice();
};

