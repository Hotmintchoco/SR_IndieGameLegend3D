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
#include "CSoundMgr.h"
#include "CIntro_MG1.h"
#include "CGameMachine.h"

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

    s_bClear = false;
    return S_OK;
}

_int CMiniGame1::Update_Scene(_float fTimeDelta)
{
    _int iExit = CScene::Update_Scene(fTimeDelta);
    m_fElapsedTime += fTimeDelta;
    if (m_bBGMStart == false && m_fElapsedTime>2.0f)
    {
        m_bBGMStart = true;
        TBGMTrack tTrack = { L"MG1", L"", L"SectorB.wav" };
        CSoundMgr::GetInstance()->PlayBGM(tTrack);
        CSoundMgr::GetInstance()->SetBGMVolume(0.8f);
        CSoundMgr::GetInstance()->SetSFXVolume(0.5f);
    }
    // Scene Change
    if (CDInputMgr::GetInstance()->Key_Down(DIK_F2))
    {
        if (CGameMachine* pGameMachine = dynamic_cast<CGameMachine*>(CManagement::GetInstance()->Get_Scene(0)->Get_GameObject(L"Room_2_Layer", L"Room_2_ObjectTiling_71")))
        {
            pGameMachine->Set_Clear();
        }
        if (FAILED(CManagement::GetInstance()->Change_Scene(0, nullptr, true)))
            return -1;
    }
    if (s_bClear == true)
    {
        m_fElapsedTime2 += fTimeDelta;
        if (m_bClosing == false)
        {   
            m_bClosing = true;
            CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
            CGameObject* pGameObject = CIntro_MG1::Create(m_pGraphicDev);
            if (nullptr == pGameObject) return E_FAIL;
            if (FAILED(pLayer->Add_GameObject(L"Intro", pGameObject))) return E_FAIL;
        }
        if (m_fElapsedTime2 > 1.4f)
        {
            if (CGameMachine* pGameMachine = dynamic_cast<CGameMachine*>(CManagement::GetInstance()->Get_Scene(0)->Get_GameObject(L"Room_2_Layer", L"Room_2_ObjectTiling_71")))
            {
                pGameMachine->Set_Clear();
            }
            if (FAILED(CManagement::GetInstance()->Change_Scene(0, nullptr, true)))
                return -1;
        }
    }
    return iExit;
}

void CMiniGame1::LateUpdate_Scene(_float fTimeDelta)
{
    CScene::LateUpdate_Scene(fTimeDelta);
}

void CMiniGame1::Render_Scene()
{
    //RenderImGui();
    //_vec2 vTitlePos{ 40.f, 40.f };
    //CFontMgr::GetInstance()->Render_Font(L"Font_Dialogue", L"Mini Game", &vTitlePos,
    //    D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
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

    //
    _vec3 vPos{2.f,0.f,8.f};
    _vec3 vScale{0.75f,0.75f,0.75f};
    
    pGameObject = CTest1::Create(m_pGraphicDev, 0, vScale, vPos);
    if (nullptr == pGameObject) return E_FAIL;
    if (FAILED(pLayer->Add_GameObject(L"Test1", pGameObject))) return E_FAIL;

    vPos.y += 1.5f;
    vScale = { 2.f,1.f,2.f };
    pGameObject = CTest1::Create(m_pGraphicDev, 1, vScale, vPos);
    if (nullptr == pGameObject) return E_FAIL;
    if (FAILED(pLayer->Add_GameObject(L"Test1", pGameObject))) return E_FAIL;

    vPos = {-14.f,3.5f,4.f};
    vScale = { 0.75f,0.75f,0.75f };
    pGameObject = CTest1::Create(m_pGraphicDev, 3, vScale, vPos);
    if (nullptr == pGameObject) return E_FAIL;
    if (FAILED(pLayer->Add_GameObject(L"Test1", pGameObject))) return E_FAIL;

    vPos.y += 1.f;
    vScale = { 1.5f,1.0f,1.5f };
    pGameObject = CTest1::Create(m_pGraphicDev, 2, vScale, vPos);
    if (nullptr == pGameObject) return E_FAIL;
    if (FAILED(pLayer->Add_GameObject(L"Test1", pGameObject))) return E_FAIL;

    // Player
    m_pPlayer = CPlayer_MG1::Create(m_pGraphicDev);
    if (nullptr == m_pPlayer) return E_FAIL;
    if (FAILED(pLayer->Add_GameObject(L"Player", m_pPlayer))) return E_FAIL;
    CTransform* pPlayerTransformCom22 = dynamic_cast<CTransform*>(Get_Component(ID_DYNAMIC,
        L"GameLogic_Layer", L"Player", L"Com_Transform"));
    pPlayerTransformCom22->Set_Pos(_vec3{ 4.f, 0.5f - 1.f, 4.f });


    //Box
    _vec3 vDir{ 0.f,-1.f,0.f };
    vPos = { 4.f, -1.f, 4.f }; //Box1
    pGameObject = CBox_MG1::Create(m_pGraphicDev, vPos, 4, vDir);
    if (nullptr == pGameObject) return E_FAIL;
    if (FAILED(pLayer->Add_GameObject(L"Box1", pGameObject))) return E_FAIL;
    m_pPlayer->Set_Box1(static_cast<CBox_MG1*>(pGameObject));

    vPos = { -5.f, 1.f, 4.f }; //Box3
    pGameObject = CBox_MG1::Create(m_pGraphicDev, vPos, 4, vDir);
    if (nullptr == pGameObject) return E_FAIL;
    if (FAILED(pLayer->Add_GameObject(L"Box1", pGameObject))) return E_FAIL;
    m_pPlayer->Set_Box3(static_cast<CBox_MG1*>(pGameObject));

    //2층
    vPos = { -1.f, 1.f, 4.f };
    for (int i = 1; i < 15; ++i)
    {
        for (int j = 3; j < 6; ++j)
        {
            for (int k = 0; k < 2; ++k)
            {
                vPos = { (_float)-i, (_float)k, (_float)j };
                _vec3 vDir{ 0.f,-1.f,0.f };
                if (i == 1 && j == 4 && k == 1)
                {
                    pGameObject = CBox_MG1::Create(m_pGraphicDev, vPos, 6, vDir);
                    if (nullptr == pGameObject) return E_FAIL;
                    if (FAILED(pLayer->Add_GameObject(L"Box", pGameObject))) return E_FAIL;
                    m_pPlayer->Set_Box2(static_cast<CBox_MG1*>(pGameObject));
                }
                else
                {
                    pGameObject = CBox_MG1::Create(m_pGraphicDev, vPos, 0, vDir);
                    if (nullptr == pGameObject) return E_FAIL;
                    if (FAILED(pLayer->Add_GameObject(L"Box", pGameObject))) return E_FAIL;
                }
            }
        }
    }

    for (int i = 1; i < 15; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            for (int k = 0; k < 1; ++k)
            {
                vPos = { (_float)-i, (_float)0.5f, (_float)5.5f - (_float)j * 3.f };
                _vec3 vDir{ 0.f,0.f,0.f };

                pGameObject = CBox_MG1::Create(m_pGraphicDev, vPos, 0, vDir);
                if (nullptr == pGameObject) return E_FAIL;
                if (FAILED(pLayer->Add_GameObject(L"Box", pGameObject))) return E_FAIL;
            }
        }
    }

    for (int i = 0; i < 2; ++i)
    {
        for (int j = 3; j < 6; ++j)
        {
            for (int k = 0; k < 1; ++k)
            {
                vPos = { -0.5f - 14.f*(_float)i, (_float)0.5f, (_float)j };
                _vec3 vDir{ 1.f,0.f,0.f };

                pGameObject = CBox_MG1::Create(m_pGraphicDev, vPos, 0, vDir);
                if (nullptr == pGameObject) return E_FAIL;
                if (FAILED(pLayer->Add_GameObject(L"Box", pGameObject))) return E_FAIL;
            }
        }
    }

    //3층
    vPos = { -1.f, 1.f, 4.f };
    for (int i = 10; i < 15; ++i)
    {
        for (int j = 3; j < 6; ++j)
        {
            for (int k = 2; k < 4; ++k)
            {
                vPos = { (_float)-i, (_float)k, (_float)j };
                _vec3 vDir{ 0.f,-1.f,0.f };
                if (i == 10 && j == 4 && k == 3)
                {
                    pGameObject = CBox_MG1::Create(m_pGraphicDev, vPos, 6, vDir);
                    if (nullptr == pGameObject) return E_FAIL;
                    if (FAILED(pLayer->Add_GameObject(L"Box", pGameObject))) return E_FAIL;
                    m_pPlayer->Set_Box4(static_cast<CBox_MG1*>(pGameObject));
                }
                else
                {
                    pGameObject = CBox_MG1::Create(m_pGraphicDev, vPos, 0, vDir);
                    if (nullptr == pGameObject) return E_FAIL;
                    if (FAILED(pLayer->Add_GameObject(L"Box", pGameObject))) return E_FAIL;
                }
            }
        }
    }

    for (int i = 10; i < 15; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            for (int k = 0; k < 1; ++k)
            {
                vPos = { (_float)-i, (_float)2.5f, (_float)5.5f - (_float)j * 3.f };
                _vec3 vDir{ 0.f,0.f,0.f };

                pGameObject = CBox_MG1::Create(m_pGraphicDev, vPos, 0, vDir);
                if (nullptr == pGameObject) return E_FAIL;
                if (FAILED(pLayer->Add_GameObject(L"Box", pGameObject))) return E_FAIL;
            }
        }
    }

    for (int i = 0; i < 2; ++i)
    {
        for (int j = 3; j < 6; ++j)
        {
            for (int k = 0; k < 1; ++k)
            {
                vPos = { -9.5f - 5.f * (_float)i, (_float)2.5f, (_float)j };
                _vec3 vDir{ 1.f,0.f,0.f };

                pGameObject = CBox_MG1::Create(m_pGraphicDev, vPos, 0, vDir);
                if (nullptr == pGameObject) return E_FAIL;
                if (FAILED(pLayer->Add_GameObject(L"Box", pGameObject))) return E_FAIL;
            }
        }
    }

    //맨밑
    vPos = { 4.f, -1.f, 4.f };
    for (int i = -20; i < 8; ++i)
    {
        for (int j = -3; j < 4; ++j)
        {
            _vec3 vDir{ 0.f,-1.f,0.f };
            _vec3 vPos2 = { (_float)i, 0.f, (_float)j };
            _vec3 vPos3 = vPos + vPos2;
            pGameObject = CBox_MG1::Create(m_pGraphicDev, vPos3, 2, vDir);
            if (nullptr == pGameObject) return E_FAIL;
            if (FAILED(pLayer->Add_GameObject(L"Box", pGameObject))) return E_FAIL;
        }
    }
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

    pGameObject = CIntro_MG1::Create(m_pGraphicDev);
    if (nullptr == pGameObject) return E_FAIL;
    if (FAILED(pLayer->Add_GameObject(L"Intro", pGameObject))) return E_FAIL;
    return S_OK;
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