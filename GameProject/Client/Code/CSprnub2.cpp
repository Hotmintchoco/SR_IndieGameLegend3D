#include "pch.h"
#include "CSprnub2.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CTimerMgr.h"
#include "CTerrain.h"
#include "CSmallExplode.h"
#include "CAbstractFactory.h"
#include "CHeart.h"
#include "CGem.h"
#include "CEnergy.h"

CSprnub2::CSprnub2(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev)
{
}


CSprnub2::~CSprnub2()
{
}

HRESULT CSprnub2::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    CMonster::Ready_GameObject();

    m_pTransformCom->Set_Scale(0.5f, 0.5f, 0.5f);
    m_pColliderCom->Set_Radius(m_pTransformCom->m_vScale.x);
    m_iHp = 2;
    return S_OK;
}

_int CSprnub2::Update_GameObject(const _float& fTimeDelta)
{
    if (m_iHp <= 0)
    {
        m_bDelete = true;

        CGameObject* pGameObject = nullptr;
        CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");

        pGameObject = CSmallExplode::Create(m_pGraphicDev, m_pTransformCom->m_vInfo[INFO_POS], m_pTransformCom->m_vScale);
        if (nullptr == pGameObject)
            return E_FAIL;

        if (FAILED(pLayer->Add_GameObject(L"SmallExplode", pGameObject)))
            return E_FAIL;

        //pGameObject = CHeart::Create(m_pGraphicDev, this);
        pGameObject = CGem::Create(m_pGraphicDev, this);
        //pGameObject = CEnergy::Create(m_pGraphicDev, this);
        if (nullptr == pGameObject)
            return E_FAIL;

        if (FAILED(pLayer->Add_GameObject(L"Gem", pGameObject)))
            return E_FAIL;
    }
    _int    iExit = CMonster::Update_GameObject(fTimeDelta);


    Set_OnTerrain();
    m_fFrame += fTimeDelta * 5.f;
    if (m_fFrame > 2.f)
        m_fFrame = 0.f;



    return iExit;
}

void CSprnub2::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CMonster::LateUpdate_GameObject(fTimeDelta);



    CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
        ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));

    if (nullptr == pPlayerTransformCom)
        return;

    _vec3   vPlayerPos;
    pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);

    _vec3   vPlayerLook;
    pPlayerTransformCom->Get_Info(INFO_LOOK, &vPlayerLook);

    if (m_fActiveElapsedTime < m_fActiveTime)
    {
        m_fActiveElapsedTime += fTimeDelta;
        m_pTransformCom->LookAt_Player(&vPlayerPos, &vPlayerLook);

    }
    else
    {
        m_pTransformCom->Chase_Target(&vPlayerPos, &vPlayerLook, 2.f, fTimeDelta);
    }


}

void CSprnub2::Render_GameObject()
{

    if (m_bHitState == true) CMonster::Enable_HitRenderState();

    CMonster::Render_GameObject();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());


    m_pTextureCom->Set_Texture((_uint)m_fFrame);


    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    if (m_bHitState == true) CMonster::Disable_HitRenderState();
}

void CSprnub2::OnCollisionEnter(CGameObject* pOther)
{
    CMonster::OnCollisionEnter(pOther);
}

HRESULT CSprnub2::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_sprnub2Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}


CSprnub2* CSprnub2::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CSprnub2* pMonster = new CSprnub2(pGraphicDev);

    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("CSprnub2 Create Failed");
        return nullptr;
    }

    return pMonster;
}

CSprnub2* CSprnub2::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fActiveTime)
{
    CSprnub2* pMonster = new CSprnub2(pGraphicDev);
    pMonster->Set_ActiveTime(fActiveTime);

    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("CSprnub3 Create Failed");
        return nullptr;
    }

    return pMonster;
}

void CSprnub2::Free()
{
    CMonster::Free();
}