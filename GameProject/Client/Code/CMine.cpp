#include "pch.h"
#include "CMine.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CTimerMgr.h"
#include "CTerrain.h"
#include "CSmallExplode.h"
#include "CAbstractFactory.h"

CMine::CMine(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev)
{
}


CMine::~CMine()
{
}

HRESULT CMine::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    CMonster::Ready_GameObject();

    m_pTransformCom->Set_Scale(0.25f, 0.25f, 0.25f);
    //m_pColliderCom->Set_Radius(m_pTransformCom->m_vScale.x);
    m_pColliderCom->Set_Radius(0.35f);

    m_iMaxHp = 3;
    m_iHp = m_iMaxHp;
    return S_OK;
}

_int CMine::Update_GameObject(_float fTimeDelta)
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
    }
    _int    iExit = CMonster::Update_GameObject(fTimeDelta);

    Set_OnTerrain();
    m_fFrame += fTimeDelta * 6.f;
    if (m_fFrame >= 2.f)
        m_fFrame = 0.f;

    return iExit;
}

void CMine::LateUpdate_GameObject(_float fTimeDelta)
{
    CMonster::LateUpdate_GameObject(fTimeDelta);

    LookAtPlayer2();
}

void CMine::Render_GameObject()
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

void CMine::OnCollisionEnter(COLLINFO eCollInfo)
{
    CMonster::OnCollisionEnter(eCollInfo);

    CCollider* pCollider = eCollInfo.pOtherCollider;
    if (pCollider && pCollider->Get_CollisionID() == COLL_PLAYER)
    {
        m_bHitState = true;
        m_fHitEffectElapsedTime = 0.f;
        m_iHp = 0;
    }
}

HRESULT CMine::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_MineTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}


CMine* CMine::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMine* pMonster = new CMine(pGraphicDev);

    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("CMine Create Failed");
        return nullptr;
    }

    return pMonster;
}

void CMine::Free()
{
    CMonster::Free();
}