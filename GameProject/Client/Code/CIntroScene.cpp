#include "pch.h"
#include "CIntroScene.h"
#include "CLogo.h"
#include "CManagement.h"
#include "CDInputMgr.h"
#include "CFontMgr.h"
#include "CCursorPolicyMgr.h"
#include "CSoundMgr.h"
#include "CProtoMgr.h"
#include "CUI.h"

CIntroScene::CIntroScene(LPDIRECT3DDEVICE9 pGraphicDev)
    : CScene(pGraphicDev)
{
}

HRESULT CIntroScene::Ready_Scene()
{
    if (FAILED(Ready_Prototype()))
        return E_FAIL;
    return Ready_UI_Layer();
}

HRESULT CIntroScene::Ready_Prototype()
{
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_IntroChatWindow", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Intro/ChatWindow.png",1))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_IntroBean", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Intro/bean_%d.png", 2))))
        return E_FAIL;

    return S_OK;
}

HRESULT CIntroScene::Ready_UI_Layer()
{
    CLayer* pLayer = CLayer::Create();
    if (!pLayer) 
        return E_FAIL;

    const float fScale = min(WINCX / 1280.f, WINCY / 720.f);
    const float fLeft = WINCX * 0.5f - 480.f * fScale;
    const float fTop = 112.f * fScale;

    auto* pWindow = CUI::Create(m_pGraphicDev, L"Proto_IntroChatWindow");

    if (!pWindow) 
        return E_FAIL;

    pWindow->Set_Size({ 480.f * fScale, 85.f * fScale });
    pWindow->Set_Pos(WINCX * 0.5f, fTop + 85.f * fScale, 0.9f);
    if (FAILED(pLayer->Add_GameObject(L"ChatWindow", pWindow)))
    {
        Safe_Release(pWindow);
        return E_FAIL;
    }

    m_pPortrait = CUI::Create(m_pGraphicDev, L"Proto_IntroBean");

    if (!m_pPortrait) 
        return E_FAIL;

    m_pPortrait->Set_Size({ 74.f * fScale, 74.f * fScale });
    m_pPortrait->Set_Pos(fLeft + 85.f * fScale, fTop + 85.f * fScale, 0.8f);
    if (FAILED(pLayer->Add_GameObject(L"Commander", m_pPortrait)))
    {
        Safe_Release(m_pPortrait);
        return E_FAIL;
    }

    m_mapLayer.insert({ L"Dialogue_Layer", pLayer });

    return S_OK;
}

void CIntroScene::OnEnter()
{
    CCursorPolicyMgr::GetInstance()->Set_MenuMode(true);
}

void CIntroScene::OnExit()
{
    CSoundMgr::GetInstance()->StopBGM();
    CCursorPolicyMgr::GetInstance()->Set_MenuMode(false);
}

_int CIntroScene::Update_Scene(_float fTimeDelta)
{
    m_pPortrait->Set_Texture(static_cast<_uint>((GetTickCount64() / 360) % 2));

    auto* pInput = CDInputMgr::GetInstance();
    if (pInput->Key_Down(DIK_RETURN) || pInput->Key_Down(DIK_SPACE) || pInput->Key_Down(DIK_ESCAPE))
        Finish_Intro();

    if (!m_bFinishRequested)
        return CScene::Update_Scene(fTimeDelta);

    m_bFinishRequested = false;
    CLogo* pLogo = CLogo::Create(m_pGraphicDev);
    if (!pLogo)
    {
        m_bLoadingFailed = true;
        return 0;
    }

    return CManagement::GetInstance()->Change_Scene(0, pLogo, true);
}

void CIntroScene::LateUpdate_Scene(_float fTimeDelta)
{
    CScene::LateUpdate_Scene(fTimeDelta);
}

void CIntroScene::Render_Scene()
{
    const float fScale = min(WINCX / 1280.f, WINCY / 720.f);
    const float fLeft = WINCX * 0.5f - 480.f * fScale;
    const _vec2 vTitle{ fLeft + 210.f * fScale, 142.f * fScale };
    const _vec2 vGuide{ fLeft, 325.f * fScale };
    CFontMgr::GetInstance()->Render_Font(L"Font_Dialogue",
        L"임무에 대한 설명은 들었나?\n출발할 준비를 하게.", &vTitle,
        D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
    CFontMgr::GetInstance()->Render_Font(L"Font_Dialogue",
        m_bLoadingFailed ? L"Loading failed. Press ENTER to retry."
                         : L"ENTER / SPACE : 건너뛰기   ESC : 종료",
        &vGuide, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
}

CIntroScene* CIntroScene::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CIntroScene* pScene = new CIntroScene(pGraphicDev);
    if (FAILED(pScene->Ready_Scene()))
    {
        Safe_Release(pScene);
        return nullptr;
    }
    return pScene;
}

void CIntroScene::Free()
{
    CScene::Free();
}
