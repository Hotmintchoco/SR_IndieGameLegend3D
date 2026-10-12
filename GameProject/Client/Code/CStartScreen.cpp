#include "pch.h"
#include "CStartScreen.h"
#include "CIntroScene.h"
#include "CManagement.h"
#include "CProtoMgr.h"
#include "CCursorPolicyMgr.h"
#include "CDInputMgr.h"
#include "CFontMgr.h"
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

    Engine::TBGMTrack tTrack{};
    tTrack.wstrID = L"Title";
    tTrack.wstrIntro = L"";
    tTrack.wstrLoop = L"Title.wav";

    CSoundMgr::GetInstance()->PlayBGM(tTrack);
    CSoundMgr::GetInstance()->SetBGMVolume(0.3f);
}

void CStartScreen::OnExit()
{
    CCursorPolicyMgr::GetInstance()->Set_MenuMode(false);
}

_int CStartScreen::Update_Scene(_float fTimeDelta)
{
    if (m_bStartFailed || !CDInputMgr::GetInstance()->Key_Down(DIK_RETURN))
        return CScene::Update_Scene(fTimeDelta);

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
    const float fScale = min(WINCX / 1280.f, WINCY / 720.f);

    const _vec2 vTextPos{ WINCX * 0.5f - 80.f, WINCY * 0.62f + 32.f * fScale - 16.f };
    CFontMgr::GetInstance()->Render_Font(L"Font_Dialogue", L"Game Start",
        &vTextPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

    if (m_bStartFailed)
    {
        const _vec2 vErrorPos{ WINCX * 0.5f - 360.f, vTextPos.y + 40.f };
        CFontMgr::GetInstance()->Render_Font(L"Font_Dialogue",
            L"Intro could not start. Please restart the game.",
            &vErrorPos, D3DXCOLOR(1.f, 0.3f, 0.3f, 1.f));
    }
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
    const float fWidth = 650.f * fScale;
    const float fHeight = fWidth * 128.f / 200.f; // title3d.png: 200 x 112
    pTitle->Set_Size({ fWidth * 0.5f, fHeight * 0.5f });
    pTitle->Set_Pos(WINCX * 0.5f, WINCY * 0.1f + fHeight * 0.5f, 0.8f);
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
