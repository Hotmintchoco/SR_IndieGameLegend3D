#include "pch.h"
#include "CMiniGame1.h"
#include "CBackGround.h"
#include "CImGuiTool.h"
#include "CFontMgr.h"
#include "CLayer.h"
#include "CProtoMgr.h"
#include "CDInputMgr.h"
#include "CManagement.h"
#include "CSkyBox.h"
#include "CTest1.h"
#include "CClientCameraMgr.h"
#include "CCamera_MG1.h"
#include "CCamera2_MG1.h"
#include "CPlayer_MG1.h"
#include "CTimerMgr.h"
#include "CBox_MG1.h"

CMiniGame1::CMiniGame1(LPDIRECT3DDEVICE9 pGraphicDev)
    : CScene(pGraphicDev)
{
}

CMiniGame1::~CMiniGame1()
{
}

HRESULT CMiniGame1::Ready_Scene()
{
    if (FAILED(Ready_Environment_Layer(L"Environment_Layer")))
        return E_FAIL;

    if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer")))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer(L"UI_Layer")))
        return E_FAIL;

    //if (FAILED(Ready_Camera()))
    //    return E_FAIL;

    return S_OK;
}

_int CMiniGame1::Update_Scene(_float fTimeDelta)
{
    _int iExit = CScene::Update_Scene(fTimeDelta);

    // Scene Change
    if (CDInputMgr::GetInstance()->Key_Down(DIK_F2))
    {
        this;
        if (FAILED(CManagement::GetInstance()->Change_Scene(0, nullptr, true)))
            return -1;
    }
    //Update_Input(fTimeDelta);
    return iExit;
}

void CMiniGame1::LateUpdate_Scene(_float fTimeDelta)
{
    CScene::LateUpdate_Scene(fTimeDelta);
}

void CMiniGame1::Render_Scene()
{
    RenderImGui();


    _vec2 vTitlePos{ 40.f, 40.f };
    CFontMgr::GetInstance()->Render_Font(L"Font_Default", L"Mini Game", &vTitlePos,
        D3DXCOLOR(1.f, 1.f, 1.f, 1.f));


}

HRESULT CMiniGame1::Ready_Environment_Layer(const _tchar* pLayerTag)
{
    CLayer* pLayer = CLayer::Create();
    if (nullptr == pLayer) return E_FAIL;
    pLayer->Set_IsActive(true);

    CGameObject* pGameObject = nullptr;

    // SkyBox
    pGameObject = CSkyBox::Create(m_pGraphicDev);
    if (nullptr == pGameObject) return E_FAIL;
    if (FAILED(pLayer->Add_GameObject(L"SkyBox", pGameObject))) return E_FAIL;


    m_mapLayer.insert({ pLayerTag ,pLayer });

    return S_OK;
}

HRESULT CMiniGame1::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
    CLayer* pLayer = CLayer::Create();
    if (nullptr == pLayer) return E_FAIL;
    //pLayer->Set_IsActive(true);
    m_mapLayer.insert({ pLayerTag ,pLayer });

    CGameObject* pGameObject = nullptr;

    // Test1
    pGameObject = CTest1::Create(m_pGraphicDev);
    if (nullptr == pGameObject) return E_FAIL;
    if (FAILED(pLayer->Add_GameObject(L"Test1", pGameObject))) return E_FAIL;

    // Player
    m_pPlayer = CPlayer_MG1::Create(m_pGraphicDev);
    if (nullptr == m_pPlayer) return E_FAIL;
    if (FAILED(pLayer->Add_GameObject(L"Player", m_pPlayer))) return E_FAIL;

    _vec3 vPos;
    //Box
    vPos = { 1.f, 0.5f, 4.f };
    pGameObject = CBox_MG1::Create(m_pGraphicDev, vPos);
    if (nullptr == pGameObject) return E_FAIL;
    if (FAILED(pLayer->Add_GameObject(L"Box1", pGameObject))) return E_FAIL;
    m_pPlayer->Set_Box1(static_cast<CBox_MG1*>(pGameObject));

    vPos = { -1.f, 1.f, 4.f };
    pGameObject = CBox_MG1::Create(m_pGraphicDev, vPos);
    if (nullptr == pGameObject) return E_FAIL;
    if (FAILED(pLayer->Add_GameObject(L"Box2", pGameObject))) return E_FAIL;
    m_pPlayer->Set_Box2(static_cast<CBox_MG1*>(pGameObject));


    // Camera1
    CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Get_Component(ID_DYNAMIC,
        L"GameLogic_Layer", L"Player", L"Com_Transform"));
    pGameObject = CCamera_MG1::Create(m_pGraphicDev, pPlayerTransformCom);
    if (nullptr == pGameObject) return E_FAIL;
    if (FAILED(pLayer->Add_GameObject(L"Camera1", pGameObject))) return E_FAIL;
    m_pPlayer->Set_Camera1(static_cast<CCamera*>(pGameObject));
    pGameObject->Set_IsActive(true);

    // Camera2
    _vec3 vEye, vAt;
    _vec3 vUp{ 0.f, 1.f, 0.f };
    pPlayerTransformCom->Get_Info(INFO_POS, &vEye);
    pPlayerTransformCom->Get_Info(INFO_LOOK, &vAt);
    pGameObject = CCamera2_MG1::Create(m_pGraphicDev, &vEye, &vAt, &vUp);
    if (nullptr == pGameObject) return E_FAIL;
    if (FAILED(pLayer->Add_GameObject(L"Camera2", pGameObject))) return E_FAIL;
    m_pPlayer->Set_Camera2(static_cast<CCamera*>(pGameObject));
    pGameObject->Set_IsActive(false);

    //m_mapLayer.insert({ pLayerTag ,pLayer });
    return S_OK;
}

void CMiniGame1::Update_Input(const _float& fTimeDelta)
{
}

void CMiniGame1::RenderImGui()
{
    ImGui::Begin("Debug");

    // --- FPS ---
    if (ImGui::CollapsingHeader("FPS", ImGuiTreeNodeFlags_DefaultOpen))
    {
        _float fDT = CTimerMgr::GetInstance()->Get_TimeDelta(L"Timer_FPS60");
        ImGui::Text("FPS : %d", (int)(1.f / fDT));
    }
    // --- Player ---
    if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen))
    {
        CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Get_Component(ID_DYNAMIC,
            L"GameLogic_Layer", L"Player", L"Com_Transform"));
        

        CPlayer_MG1* pPlayer = static_cast<CPlayer_MG1*>(CManagement::GetInstance()->Get_GameObject(L"GameLogic_Layer", L"Player"));
        _vec3 vPos; pPlayerTransformCom->Get_Info(INFO_POS, &vPos);
        _vec3 vAngle = pPlayerTransformCom->Get_Angle();
        ImGui::Text("Pos : %.2f, %.2f, %.2f", vPos.x, vPos.y, vPos.z);
        ImGui::Text("Pitch : %.2f, Raw %.2f", vAngle.x, vAngle.y);
        ImGui::Text("Connection : %d", pPlayer->Is_Connection());

    }        

    ImGui::End();
}

CMiniGame1* CMiniGame1::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMiniGame1* pMiniGame1 = new CMiniGame1(pGraphicDev);
    if (FAILED(pMiniGame1->Ready_Scene()))
    {
        Safe_Release(pMiniGame1);
        MSG_BOX("CMiniGame1 Create Failed");
        return nullptr;
    }

    return pMiniGame1;
}

void CMiniGame1::Free()
{
    CScene::Free();
}