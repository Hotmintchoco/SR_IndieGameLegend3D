#include "pch.h"
#include "CSkull.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CTimerMgr.h"
#include "CTerrain.h"
#include "CPlayerCamera.h"

CSkull::CSkull(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev)
{
}


CSkull::~CSkull()
{
}

HRESULT CSkull::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    CMonster::Ready_GameObject();

    m_pTransformCom->Set_Scale(2.f, 2.f, 2.f);
    m_pColliderCom->Set_Radius(m_pTransformCom->m_vScale.x);
    m_iMaxHp = 3;
    m_iHp = m_iMaxHp;
    return S_OK;
}

_int CSkull::Update_GameObject(_float fTimeDelta)
{
    if (m_iHp <= 0)
    {
        m_bDelete = true;
    }

    _int    iExit = CMonster::Update_GameObject(fTimeDelta);
    Set_OnTerrain();



    return iExit;
}

void CSkull::LateUpdate_GameObject(_float fTimeDelta)
{
    CMonster::LateUpdate_GameObject(fTimeDelta);

    CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
        ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));

    if (nullptr == pPlayerTransformCom)
        return;

    const TBillBoardInfo& tInfo = m_pBillBoardCamera->GetBillBoardInfo();

    _vec3   vPlayerPos;
    pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);

    _vec3   vPlayerLook;
    vPlayerLook = tInfo.vLook;

    m_pTransformCom->Chase_Target2(&vPlayerPos, &vPlayerLook, 30.f, fTimeDelta);

}

void CSkull::Render_GameObject()
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

void CSkull::OnCollisionEnter(COLLINFO eCollInfo)
{
    CMonster::OnCollisionEnter(eCollInfo);
}

HRESULT CSkull::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_skull3Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}


CSkull* CSkull::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CSkull* pMonster = new CSkull(pGraphicDev);

    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("CSkull Create Failed");
        return nullptr;
    }

    return pMonster;
}

void CSkull::Free()
{
    CMonster::Free();
}