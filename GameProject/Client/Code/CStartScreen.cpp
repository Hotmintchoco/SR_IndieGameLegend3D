#include "pch.h"
#include "CStartScreen.h"
#include "CIntroScene.h"
#include "CManagement.h"
#include "CProtoMgr.h"
#include "CCursorPolicyMgr.h"
#include "CImGuiTool.h"
#include "CUI.h"
#include "CSoundMgr.h"

CStartScreen::CStartScreen(LPDIRECT3DDEVICE9 pGraphicDev)
    : CScene(pGraphicDev)
{
}

HRESULT CStartScreen::Ready_Scene()
{
    if (FAILED(Ready_Prototype()))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer(L"UI_Layer")))
        return E_FAIL;

    if (FAILED(CSoundMgr::GetInstance()->LoadSound(L"../Bin/Resource/Sound/bgm/")))
        return E_FAIL;

    return S_OK;
}

void CStartScreen::OnEnter()
{
    CCursorPolicyMgr::GetInstance()->Set_MenuMode(true);
    CSoundMgr::GetInstance()->PlayBGM(L"Title.wav");
    CSoundMgr::GetInstance()->SetBGMVolume(0.3f);
}

void CStartScreen::OnExit()
{
    // Keep title music playing through the intro; CIntroScene stops it on exit.
    CCursorPolicyMgr::GetInstance()->Set_MenuMode(false);
}

_int CStartScreen::Update_Scene(_float fTimeDelta)
{
    if (!m_bStartRequested)
        return CScene::Update_Scene(fTimeDelta);

    m_bStartRequested = false;
    CIntroScene* pIntro = CIntroScene::Create(m_pGraphicDev);
    if (!pIntro)
    {
        m_bStartFailed = true;
        return 0;
    }

    return CManagement::GetInstance()->Change_Scene(0, pIntro, true);
}

void CStartScreen::LateUpdate_Scene(_float fTimeDelta)
{
    CScene::LateUpdate_Scene(fTimeDelta);
}

void CStartScreen::Render_Scene()
{
    const ImGuiViewport* pViewport = ImGui::GetMainViewport();
    const ImVec2 vSize = pViewport->Size;
    const float fScale = min(vSize.x / 1280.f, vSize.y / 720.f);
    const ImVec2 vButtonSize(280.f * fScale, 64.f * fScale);
    ImGui::SetNextWindowPos(ImVec2(pViewport->Pos.x + vSize.x * 0.5f,
        pViewport->Pos.y + vSize.y * 0.72f), ImGuiCond_Always, ImVec2(0.5f, 0.f));
    ImGui::SetNextWindowViewport(pViewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_AlwaysAutoResize;

    if (ImGui::Begin("Game Start Screen", nullptr, flags))
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.02f, 0.20f, 0.32f, 0.95f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.02f, 0.42f, 0.62f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.02f, 0.30f, 0.46f, 1.f));
        ImGui::BeginDisabled(m_bStartRequested || m_bStartFailed);
        if (ImGui::Button("START GAME", vButtonSize))
            m_bStartRequested = true;
        ImGui::EndDisabled();
        ImGui::PopStyleColor(3);

        if (m_bStartFailed)
        {
            ImGui::TextUnformatted("Intro could not start. Please restart the game.");
        }
    }
    ImGui::End();
    ImGui::PopStyleVar(3);
}

HRESULT CStartScreen::Ready_Prototype()
{
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_StartBackgroundTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Logo/SkyboxStars.png", 1))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_StartTitleTexture",CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Logo/title3d.png", 1))))
        return E_FAIL;

    return S_OK;
}

HRESULT CStartScreen::Ready_UI_Layer(const _tchar* pLayerTag)
{
    CLayer* pLayer = CLayer::Create();
    if (!pLayer)
        return E_FAIL;
    m_mapLayer.insert({ pLayerTag, pLayer });

    CUI* pBackground = CUI::Create(m_pGraphicDev, L"Proto_StartBackgroundTexture");
    if (!pBackground)
        return E_FAIL;

    // CRcTex spans -1 to +1, so Set_Size takes half of the displayed size.
    pBackground->Set_Size({ WINCX * 0.5f, WINCY * 0.5f });
    pBackground->Set_Pos(WINCX * 0.5f, WINCY * 0.5f, 0.9f);
    if (FAILED(pLayer->Add_GameObject(L"Background", pBackground)))
    {
        Safe_Release(pBackground);
        return E_FAIL;
    }

    CUI* pTitle = CUI::Create(m_pGraphicDev, L"Proto_StartTitleTexture");
    if (!pTitle)
        return E_FAIL;

    const float fScale = min(WINCX / 1280.f, WINCY / 720.f);
    const float fWidth = 600.f * fScale;
    const float fHeight = fWidth * 112.f / 200.f; // title3d.png: 200 x 112
    pTitle->Set_Size({ fWidth * 0.5f, fHeight * 0.5f });
    pTitle->Set_Pos(WINCX * 0.5f, WINCY * 0.15f + fHeight * 0.5f, 0.8f);
    if (FAILED(pLayer->Add_GameObject(L"Title", pTitle)))
    {
        Safe_Release(pTitle);
        return E_FAIL;
    }

    return S_OK;
}

CStartScreen* CStartScreen::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CStartScreen* pScreen = new CStartScreen(pGraphicDev);
    if (FAILED(pScreen->Ready_Scene()))
    {
        Safe_Release(pScreen);
        MSG_BOX("Start Screen Create Failed");
        return nullptr;
    }
    return pScreen;
}

void CStartScreen::Free()
{
    CScene::Free();
}
