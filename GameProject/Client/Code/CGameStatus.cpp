#include "pch.h"
#include "CGameStatus.h"
#include "CManagement.h"
#include "CRoomLayer.h"
#include "CImGuiTool.h"
#include "CClientCameraMgr.h"
#include "CCamera.h"
#include "CTransform.h"
#include "CDebugMgr.h"
#include "CRoomLoadingMgr.h"
#include "CSoundMgr.h"
#include "CVIBuffer.h"
#include "CRayCaster.h"
#include "IRayTestable.h"
#include "CTimerMgr.h"
#include "CStage.h"
#include "CRenderer.h"
#include "CPlayer.h"
#include "CPlayerCamera.h"

CGameStatus::CGameStatus(LPDIRECT3DDEVICE9 pGraphicDev)
    :CGameObject(pGraphicDev)
{
}

CGameStatus::~CGameStatus()
{
}

HRESULT CGameStatus::Ready_GameObject()
{
    return S_OK;
}

_int CGameStatus::Update_GameObject(_float fTimeDelta)
{
    UpdateCameraInfo();

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    m_fDT = CTimerMgr::GetInstance()->Get_TimeDelta(L"Timer_FPS60");

    DebugRayTest();

    return S_OK;
}

void CGameStatus::LateUpdate_GameObject(_float fTimeDelta)
{
}

void CGameStatus::Render_GameObject()
{
    RenderImGui();
    DebugPanelForRendering();
}

void CGameStatus::DebugRayTest()
{
    if (!m_bDebugTriangle) return;
    
    CStage* pStage = static_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene());

    const vector<IRayTestable*>& mapObject = pStage->GetCurrentRoomLayer()->GetRayTestableList();
    CRayCaster* pRayCaster = static_cast<CRayCaster*>(CManagement::GetInstance()->Get_GameObject(L"GameLogic_Layer", L"RayCaster"));

    THitInfo t{};

    for (auto& pObj : mapObject)
    {
        vector<pair<CVIBuffer*, CTransform*>> vecInfo = pObj->GetRayTestTargetInfo();
        for (auto& [pBuffer, pTransform] : vecInfo)
        {
            pRayCaster->RayTest(t, m_vCamPos, m_vCamLook, pBuffer, pTransform->Get_World());
        }
    }
    
    if (t.bHit)
    {
        CRenderer::GetInstance()->Add_DebugTriangle(t.vTriVtx, t.fTriNormal);
    }
}

void CGameStatus::UpdateCameraInfo()
{
    CCamera* pCamera = CClientCameraMgr::GetInstance()->Get_ActiveCamera();
    if (!pCamera) return;

    _matrix matCamWorld;
    pCamera->GetWorld(&matCamWorld);
    memcpy(&m_vCamLook, &matCamWorld.m[2][0], sizeof(_vec3));
    memcpy(&m_vCamPos, &matCamWorld.m[3][0], sizeof(_vec3));
    m_fYaw = atan2f(m_vCamLook.x, m_vCamLook.z);
}

void CGameStatus::RenderImGui()
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
    
        ImGui::Text("Yaw : %.1f deg", XMConvertToDegrees(m_fYaw)); 
    
        if(ImGui::SliderFloat("Player Scale", &m_fPseudoPlayerScale, 0.01f, 1.f, "%.2f"))
        {
            m_pStage->GetPlayer()->SetPseudoScale(m_fPseudoPlayerScale);
            CPlayerCamera* pCamera = dynamic_cast<CPlayerCamera*>(CClientCameraMgr::GetInstance()->Find_Camera(CLIENT_CAMERA_TYPE::PLAYER));
            if (pCamera)
            {
                pCamera->SetPseudoScale(m_fPseudoPlayerScale);
            }
        }
    }

    // --- Gauge ---
    if (ImGui::CollapsingHeader("Gauge", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::ProgressBar(m_fUltGauge, ImVec2(-FLT_MIN, 0.f));
        ImGui::SameLine(); ImGui::Text("Ult");

        ImGui::ProgressBar(m_fSpecialAtkGauge, ImVec2(-FLT_MIN, 0.f));
        ImGui::SameLine(); ImGui::Text("Special");
    }

    // --- Etc ---
    ImGui::Separator();
    ImGui::Text("Gem : %d", m_iGem);


    // --- Sound ---
    ImGui::SeparatorText("Sound");
    {
        CSoundMgr* pSound = CSoundMgr::GetInstance();

        // 뮤트 체크박스 + 볼륨 슬라이더 한 줄. 값이 바뀌면 true
        auto VolumeRow = [](const char* szLabel, float& fVolume, _bool& bMute) -> _bool
            {
                ImGui::PushID(szLabel);

                _bool bChanged = ImGui::Checkbox("Mute", &bMute);
                ImGui::SameLine();

                ImGui::BeginDisabled(bMute);
                bChanged |= ImGui::SliderFloat(szLabel, &fVolume, 0.f, 1.f, "%.2f");
                ImGui::EndDisabled();

                ImGui::PopID();
                return bChanged;
            };

        if (VolumeRow("BGM", m_fBGMVolume, m_bBGMMute))
            pSound->SetBGMVolume(m_bBGMMute ? 0.f : m_fBGMVolume);

        if (VolumeRow("SFX", m_fSFXVolume, m_bSFXMute))
            pSound->SetSFXVolume(m_bSFXMute ? 0.f : m_fSFXVolume);
    }

    /* TimeScale */

    ImGui::SeparatorText("Time Scale");
    {
        _bool bChanged = false;

        bChanged |= ImGui::SliderFloat("Scale", &m_fTimeScale, 0.1f, 10.f, "%.2f", ImGuiSliderFlags_Logarithmic);
        bChanged |= ImGui::Checkbox("Exclude Player", &m_bExcludePlayer);

        if (bChanged)
        {
            CTimerMgr::GetInstance()->SetGlobalTimeScale(m_fTimeScale);
            if (m_bExcludePlayer)
            {
                CTimerMgr::GetInstance()->SetGroupTimeScale(TG_PLAYER, 1.f / CTimerMgr::GetInstance()->GetGlobalTimeScale());
            }
            else
            {
                CTimerMgr::GetInstance()->SetGroupTimeScale(TG_PLAYER, 1.f);
            }
        }
    }

    /* Camera Distance */
    ImGui::SeparatorText("Camera Distance");
    {
        auto* pPlayerCamera = static_cast<CPlayerCamera*>(CClientCameraMgr::GetInstance()->Find_Camera(CLIENT_CAMERA_TYPE::PLAYER));

        if (pPlayerCamera)
        {
            _float fDistance = pPlayerCamera->Get_Distance();
            if (ImGui::SliderFloat("Distance", &fDistance, 0.1f, 20.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
            {
                pPlayerCamera->Set_Distance(fDistance);
            }
        }
    }

    ImGui::End();
}

void CGameStatus::DebugPanelForRendering()
{
    if (!ImGui::Begin("Render Debug View"))
    {
        ImGui::End();
        return;
    }

    CDebugMgr* pDebug = CDebugMgr::GetInstance();

    ImGui::SeparatorText("Mesh");

    //static const char* szMeshMode[] = { "Solid", "Wireframe", "Hidden" };
    static const char* szMeshMode[] = { "Solid", "Wireframe" };
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


    ImGui::SeparatorText("Ray");

    if (ImGui::Checkbox("Ray Cast Triangle", &m_bDebugTriangle))
    {
        CDebugMgr::GetInstance()->SetShowDebugTriangle(m_bDebugTriangle);
    }

    ImGui::End();
}


CGameStatus* CGameStatus::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CGameStatus* pObject = new CGameStatus(pGraphicDev);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("CGameStatus Create Failed");
        return nullptr;
    }

    return pObject;
}

void CGameStatus::Free()
{
}
