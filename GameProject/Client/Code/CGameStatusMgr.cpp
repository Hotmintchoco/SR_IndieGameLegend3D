#include "pch.h"
#include "CGameStatusMgr.h"
#include "CManagement.h"
#include "CRoomLayer.h"
#include "CImGuiTool.h"
#include "CCameraMgr.h"

IMPLEMENT_SINGLETON(CGameStatusMgr);

CGameStatusMgr::CGameStatusMgr()
{
}

CGameStatusMgr::~CGameStatusMgr()
{
}

void CGameStatusMgr::Update(float fTimeDelta)
{
    _vec3 vCameraLook;
    CCameraMgr::GetInstance()->Get_CamLook(&vCameraLook);
    m_fYaw = atan2f(vCameraLook.x, vCameraLook.z);
}

void CGameStatusMgr::Render()
{
    RenderImGui();
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
        ImGui::Text("Current Room : %d", m_iCurrentRoomIndex);
    }

    // --- Player ---
    if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Pos : %.2f, %.2f, %.2f", m_vPlayerPos.x, m_vPlayerPos.y, m_vPlayerPos.z);

        // HP
        char szHp[32];
        sprintf_s(szHp, "Hp : %d / %d", m_iPlayerHp, m_iPlayerMaxHp);

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

CRoomLayer* CGameStatusMgr::GetCurrentRoomLayer()
{
    wstring wstrRoomLayerKey = L"Room_" + to_wstring(m_iCurrentRoomIndex) + L"_Layer";
    CRoomLayer* pLayer = static_cast<CRoomLayer*>(CManagement::GetInstance()->Get_Layer(wstrRoomLayerKey.c_str()));
    return pLayer;
}

void CGameStatusMgr::Free()
{
}
