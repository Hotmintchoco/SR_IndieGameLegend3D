#include "pch.h"
#include "CIntroScene.h"
#include "CLogo.h"
#include "CManagement.h"
#include "CDInputMgr.h"
#include "CFontMgr.h"
#include "CCursorPolicyMgr.h"
#include "CSoundMgr.h"

CIntroScene::CIntroScene(LPDIRECT3DDEVICE9 pGraphicDev)
    : CScene(pGraphicDev)
{
}

HRESULT CIntroScene::Ready_Scene()
{
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
    // Temporary controls until dialogue progression is connected.
    auto* pInput = CDInputMgr::GetInstance();
    if (pInput->Key_Down(DIK_RETURN) || pInput->Key_Down(DIK_SPACE) ||
        pInput->Key_Down(DIK_ESCAPE))
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

    // Change_Scene releases this scene; do not access members afterwards.
    return CManagement::GetInstance()->Change_Scene(0, pLogo, true);
}

void CIntroScene::LateUpdate_Scene(_float fTimeDelta)
{
    CScene::LateUpdate_Scene(fTimeDelta);
}

void CIntroScene::Render_Scene()
{
    m_pGraphicDev->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.f, 0);

    // Placeholder only: portrait and dialogue UI will be added here later.
    const _vec2 vTitle{ WINCX * 0.15f, WINCY * 0.25f };
    const _vec2 vGuide{ WINCX * 0.15f, WINCY * 0.65f };
    CFontMgr::GetInstance()->Render_Font(L"Font_Dialogue", L"INTRO", &vTitle,
        D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
    CFontMgr::GetInstance()->Render_Font(L"Font_Dialogue",
        m_bLoadingFailed ? L"Loading failed. Press ENTER to retry."
                         : L"ENTER / SPACE : Continue     ESC : Skip",
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
