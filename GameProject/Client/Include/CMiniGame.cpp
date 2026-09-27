#include "pch.h"
#include "CMiniGame.h"
#include "CBackGround.h"
#include "CFontMgr.h"
#include "CLayer.h"
#include "CProtoMgr.h"
#include "CDInputMgr.h"
#include "CManagement.h"
#include "CStage.h"

CMiniGame::CMiniGame(LPDIRECT3DDEVICE9 pGraphicDev)
    : CScene(pGraphicDev)
{
}

CMiniGame::~CMiniGame()
{
}

HRESULT CMiniGame::Ready_Scene()
{
    if (FAILED(Ready_Environment_Layer(L"Environment_Layer")))
        return E_FAIL;

    if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer")))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer(L"UI_Layer")))
        return E_FAIL;

    return S_OK;
}

_int CMiniGame::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = CScene::Update_Scene(fTimeDelta);

    // Scene Change
    if (CDInputMgr::GetInstance()->Key_Down(DIK_F2))
    {
        CScene* pStage = CStage::Create(m_pGraphicDev);
        if (nullptr == pStage)
            return E_FAIL;

        if (FAILED(CManagement::GetInstance()->Set_Scene(pStage)))
        {
            Safe_Release(pStage);
            MSG_BOX("Stage Create Failed");
            return -1;
        }
        pStage->Update_Scene(fTimeDelta);
    }

    return iExit;
}

void CMiniGame::LateUpdate_Scene(const _float& fTimeDelta)
{
    CScene::LateUpdate_Scene(fTimeDelta);
}

void CMiniGame::Render_Scene()
{
    _vec2 vTitlePos{ 40.f, 40.f };
    CFontMgr::GetInstance()->Render_Font(L"Font_Default", L"Mini Game", &vTitlePos,
        D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
}

HRESULT CMiniGame::Ready_Environment_Layer(const _tchar* pLayerTag)
{
  CComponent* pPrototype = CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex");
    if (nullptr == pPrototype)
    {
        if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RcTex", Engine::CRcTex::Create(m_pGraphicDev))))
            return E_FAIL;
    }
    else
    {
        Safe_Release(pPrototype);
    }

    pPrototype = CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_LogoTexture");
    if (nullptr == pPrototype)
    {
        if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_LogoTexture",
            Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Logo/sana.jpg", 1))))
            return E_FAIL;
    }
    else
    {
        Safe_Release(pPrototype);
    }

    CLayer* pLayer = CLayer::Create();
    if (nullptr == pLayer)
        return E_FAIL;

    CGameObject* pGameObject = CBackGround::Create(m_pGraphicDev);
    if (nullptr == pGameObject)
    {
        Safe_Release(pLayer);
        return E_FAIL;
    }

    if (FAILED(pLayer->Add_GameObject(L"BackGround", pGameObject)))
    {
        Safe_Release(pGameObject);
        Safe_Release(pLayer);
        return E_FAIL;
    }

    m_mapLayer.insert({ pLayerTag, pLayer });
    return S_OK;
}

CMiniGame* CMiniGame::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMiniGame* pMiniGame = new CMiniGame(pGraphicDev);
    if (FAILED(pMiniGame->Ready_Scene()))
    {
        Safe_Release(pMiniGame);
        MSG_BOX("CMiniGame Create Failed");
        return nullptr;
    }

    return pMiniGame;
}

void CMiniGame::Free()
{
    CScene::Free();
}
