#include "pch.h"
#include "CMagmamouth.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CTimerMgr.h"
#include "CTerrain.h"

CMagmamouth::CMagmamouth(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev)
{
}


CMagmamouth::~CMagmamouth()
{
}

HRESULT CMagmamouth::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    CMonster::Ready_GameObject();

    m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
    m_pColliderCom->Set_Radius(m_pTransformCom->m_vScale.x);
    //m_pColliderCom->Set_Radius(0.2f);
    m_iHp = 100;
    return S_OK;
}
    
_int CMagmamouth::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CMonster::Update_GameObject(fTimeDelta);
    //Set_OnTerrain();
    m_fFrame += fTimeDelta * 6.f;
    if (m_fFrame > 4.f)
        m_fFrame = 0.f;

    return iExit;
}

void CMagmamouth::LateUpdate_GameObject(const _float& fTimeDelta)
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

    //m_pTransformCom->Chase_Target2(&vPlayerPos, &vPlayerLook, 30.f, fTimeDelta);
    m_pTransformCom->LookAt_Player(&vPlayerPos, &vPlayerLook);

}

void CMagmamouth::Render_GameObject()
{
    if (m_bHitState == true) CMonster::Enable_HitRenderState();
    CMonster::Render_GameObject();
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture(0);
    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    if (m_bHitState == true) CMonster::Disable_HitRenderState();
}

void CMagmamouth::OnCollisionEnter(CGameObject* pOther)
{
    CMonster::OnCollisionEnter(pOther);
    m_iHp -= 1;
}

HRESULT CMagmamouth::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_magmamouthTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}


CMagmamouth* CMagmamouth::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMagmamouth* pMonster = new CMagmamouth(pGraphicDev);

    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("CMagmamouth Create Failed");
        return nullptr;
    }

    return pMonster;
}

void CMagmamouth::Free()
{
    CMonster::Free();
}