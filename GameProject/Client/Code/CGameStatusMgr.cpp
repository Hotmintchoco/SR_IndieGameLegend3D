#include "pch.h"
#include "CGameStatusMgr.h"
#include "CManagement.h"
#include "CRoomLayer.h"
#include "CImGuiTool.h"
#include "CCameraMgr.h"
#include "CDebugMgr.h"

IMPLEMENT_SINGLETON(CGameStatusMgr);

CGameStatusMgr::CGameStatusMgr()
{
}

CGameStatusMgr::~CGameStatusMgr()
{
}

void CGameStatusMgr::Update(const float fTimeDelta)
{
    _vec3 vCameraLook;
    CCameraMgr::GetInstance()->Get_CamLook(&vCameraLook);
    m_fYaw = atan2f(vCameraLook.x, vCameraLook.z);
}

void CGameStatusMgr::Render()
{
    //RenderImGui();
    //DebugPanelForRendering();
}

void CGameStatusMgr::RenderImGui()
{
    ImGui::Begin("Debug");

    // --- FPS ---
    if (ImGui::CollapsingHeader("FPS", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("FPS : %d", (int)(1.f / m_fDT));
    }

    // --- Debug ---
    if (ImGui::CollapsingHeader("Stage", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Current Room : %d (%d, %d)", m_iCurrentRoomIndex,
            m_iCurrentRoomIndex / 5, m_iCurrentRoomIndex % 5);

        ImGui::Separator();

        for (int y = 0; y < 5; ++y)
        {
            for (int x = 0; x < 5; ++x)
            {
                int idx = y * 5 + x;
                if (x > 0) ImGui::SameLine();

                const char* mark = "?";
                ImVec4 col = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

                if (m_bClearTable[idx]) { mark = "O"; col = ImVec4(0.3f, 1.0f, 0.3f, 1.0f); }
                else if (m_bVisitTable[idx]) { mark = "A"; col = ImVec4(1.0f, 0.9f, 0.3f, 1.0f); }

                // 현재 방 강조
                if (idx == m_iCurrentRoomIndex)
                    col = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);

                ImGui::TextColored(col, " %s ", mark);
            }
        }
    }

    // --- Player ---
    if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Pos : %.2f, %.2f, %.2f", m_vPlayerPos.x, m_vPlayerPos.y, m_vPlayerPos.z);

        // HP
        ImGui::Text("Hp : %d / %d", m_iPlayerHp, m_iPlayerMaxHp);

        ImGui::Text("Yaw : %.1f deg", XMConvertToDegrees(m_fYaw)); 
    }

    // --- Gauge ---
    if (ImGui::CollapsingHeader("Gauge", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::ProgressBar(m_fUltGauge, ImVec2(-FLT_MIN, 0.f));
        ImGui::SameLine(); ImGui::Text("Ult");

        ImGui::ProgressBar(m_fSpecialAtkGauge, ImVec2(-FLT_MIN, 0.f));
        ImGui::SameLine(); ImGui::Text("Special");

        ImGui::SliderFloat("Ult##edit", &m_fUltGauge, 0.f, 1.f);
        ImGui::SliderFloat("Special##edit", &m_fSpecialAtkGauge, 0.f, 1.f);
    }

    // --- Etc ---
    ImGui::Separator();
    ImGui::Text("Gem : %d", m_iGem);

    ImGui::End();
}

void CGameStatusMgr::DebugPanelForRendering()
{
    if (!ImGui::Begin("Render Debug View"))
    {
        ImGui::End();
        return;
    }

    CDebugMgr* pDebug = CDebugMgr::GetInstance();

    ImGui::SeparatorText("Mesh");

    static const char* szMeshMode[] = { "Solid", "Wireframe", "Hidden" };
    _int iMeshMode = (_int)pDebug->GetMeshMode();

    if (ImGui::Combo("Render Mode", &iMeshMode, szMeshMode, IM_ARRAYSIZE(szMeshMode)))
        pDebug->SetMeshMode((MESHRENDERMODE)iMeshMode);

    ImGui::SeparatorText("Collider");

    _bool bCollider = pDebug->GetShowCollider();
    if (ImGui::Checkbox("Collider", &bCollider))
        pDebug->SetShowCollider(bCollider);

    ImGui::SeparatorText("Light");

    if (ImGui::Checkbox("Dark", &m_bShowDark))
    {
        for (auto p: m_vecPseudoDark)
        {
            p->Set_IsActive(m_bShowDark);
        }
    }

    ImGui::End();
}

CRoomLayer* CGameStatusMgr::GetCurrentRoomLayer()
{
    wstring wstrRoomLayerKey = L"Room_" + to_wstring(m_iCurrentRoomIndex) + L"_Layer";
    CRoomLayer* pLayer = static_cast<CRoomLayer*>(CManagement::GetInstance()->Get_Layer(wstrRoomLayerKey.c_str()));
    return pLayer;
}

void CGameStatusMgr::Free()
{
}
