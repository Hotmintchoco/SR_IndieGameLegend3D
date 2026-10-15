#include "pch.h"
#include "CGameMachine.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CSphereCollider.h"
#include "CPlayer.h"
#include "CCollisionMgr.h"
#include "CRoomLayer.h"
#include "CAbstractFactory.h"
#include "CManagement.h"
#include "CTimerMgr.h"
#include "CHeart.h"
#include "CGem.h"
#include "CEnergy.h"

CGameMachine::CGameMachine(LPDIRECT3DDEVICE9 pGraphicDev, ESceneType eType)
    : CGameObject(pGraphicDev), m_eSceneType(eType)
{
}

CGameMachine::~CGameMachine()
{
}

HRESULT CGameMachine::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pColliderCom->Set_Owner(this);
    m_pColliderCom->Set_Radius(1.f);


    return S_OK;
}

_int CGameMachine::Update_GameObject(_float fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    CCollisionMgr::GetInstance()->Add_Collider(COLL_ROOMLOGIC, m_pColliderCom);

    if (m_bClear == true)
    {
        m_bClear = false;
        CItem* pGameObject = nullptr;
        CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");

        for (int i = 0; i < 20; ++i)
        {
            int iRand = rand() % 100;
            if (iRand < 33)
            {
                pGameObject = CHeart::Create(m_pGraphicDev, this);
                if (nullptr == pGameObject) return E_FAIL;
                if (FAILED(pLayer->Add_GameObject(L"Heart", pGameObject))) return E_FAIL;
            }
            else if (iRand < 66)
            {
                pGameObject = CEnergy::Create(m_pGraphicDev, this);
                if (nullptr == pGameObject) return E_FAIL;
                if (FAILED(pLayer->Add_GameObject(L"Energy", pGameObject))) return E_FAIL;
            }
            else
            {
                pGameObject = CGem::Create(m_pGraphicDev, this);
                if (nullptr == pGameObject) return E_FAIL;
                if (FAILED(pLayer->Add_GameObject(L"Energy", pGameObject))) return E_FAIL;
            }

            _vec3 vDir;
            vDir.x = (_float)(rand() % 128 - 64) / 64.f;
            vDir.y = (_float)(rand() % 128) / 128.f;
            vDir.z = (_float)(rand() % 128 - 64) / 64.f;

            vDir *= 3.f;
            vDir.y += 5.f;
            pGameObject->Is_Scattering(true);
            pGameObject->Set_ScatteringDir(vDir);
        }
    }

    return iExit;
}

void CGameMachine::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CGameMachine::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Set_Texture(0);
    m_pBufferCom->Render_Buffer();
}

void CGameMachine::Interact()
{
    if (m_F)
    {
        m_F();
    }
    else
    {
        CScene* pMiniGame = CAbstractFactory::GetInstance()->CreateScene(m_eSceneType);
        if (nullptr == pMiniGame)
            return;

        if (FAILED(CManagement::GetInstance()->Change_Scene(1, pMiniGame)))
        {
            Safe_Release(pMiniGame);
            return;
        }
        pMiniGame->Update_Scene(CTimerMgr::GetInstance()->Get_FrameDelta());
    }
}

void CGameMachine::RegisterFunction(const function<void()>& F)
{
    m_F = F;
}

HRESULT CGameMachine::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Vertex
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_GameMachine_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_DownBuffer", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Collider
    pComponent = m_pColliderCom = dynamic_cast<CSphereCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SphereCollider"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_BoxCollider", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_GameMachine_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });


    return S_OK;
}

void CGameMachine::OnCollisionEnter(COLLINFO eCollInfo)
{
}

CGameMachine* CGameMachine::Create(LPDIRECT3DDEVICE9 pGraphicDev, ESceneType eType)
{
    CGameMachine* pTriggerBox = new CGameMachine(pGraphicDev, eType);

    if (FAILED(pTriggerBox->Ready_GameObject()))
    {
        Safe_Release(pTriggerBox);
        MSG_BOX("CGameMachine Create Failed");
        return nullptr;
    }

    return pTriggerBox;
}

void CGameMachine::Free()
{
    CGameObject::Free();
}
