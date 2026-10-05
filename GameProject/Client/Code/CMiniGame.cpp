#include "pch.h"
#include "CMiniGame.h"
#include "CBackGround.h"
#include "CFontMgr.h"
#include "CLayer.h"
#include "CProtoMgr.h"
#include "CDInputMgr.h"
#include "CManagement.h"
#include "CSkyBox.h"

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

_int CMiniGame::Update_Scene(_float fTimeDelta)
{
	_int iExit = CScene::Update_Scene(fTimeDelta);

    // Scene Change
    if (CDInputMgr::GetInstance()->Key_Down(DIK_F2))
    {
        if (FAILED(CManagement::GetInstance()->Change_Scene(0, nullptr, true)))
            return -1;
    }

    return iExit;
}

void CMiniGame::LateUpdate_Scene(_float fTimeDelta)
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
    CLayer* pLayer = CLayer::Create();
    if (nullptr == pLayer)
        return E_FAIL;

    // 오브젝트 추가
    CGameObject* pGameObject = nullptr;

    // SkyBox
    pGameObject = CSkyBox::Create(m_pGraphicDev);
    if (nullptr == pGameObject)
        return E_FAIL;

    if (FAILED(pLayer->Add_GameObject(L"SkyBox", pGameObject)))
        return E_FAIL;

    m_mapLayer.insert({ pLayerTag ,pLayer });

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
