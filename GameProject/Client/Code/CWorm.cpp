#include "pch.h"
#include "CWorm.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CTimerMgr.h"
//#include "CDInputMgr.h"
#include "CTerrain.h"

CWorm::CWorm(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev)
{
}


CWorm::~CWorm()
{
}

HRESULT CWorm::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    CMonster::Ready_GameObject();

    m_pTransformCom->Set_Scale(2.f, 2.f, 2.f);
    m_pTransformCom->Set_Pos(45.f, 0.f, 45.f);
    m_pColliderCom->Set_Radius(1.f);

    return S_OK;
}

_int CWorm::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CMonster::Update_GameObject(fTimeDelta);
    Set_OnTerrain();
    return iExit;
}

void CWorm::LateUpdate_GameObject(const _float& fTimeDelta)
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

    m_pTransformCom->Chase_Target(&vPlayerPos, &vPlayerLook, 3.f, fTimeDelta);

}

void CWorm::Render_GameObject()
{
    CMonster::Render_GameObject();
    //m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    //m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    //m_pTextureCom->Set_Texture(0);
    //m_pBufferCom->Render_Buffer();

    //m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CWorm::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_body_sideTexture"));
    
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_body_45Texture"));
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_body_frontTexture"));
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_body_sideTexture"));
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_body_topTexture"));
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_connectorTexture"));
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_tail_backTexture"));
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_tail_sideTexture"));
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_tail_topTexture"));
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_worm_drill_sideTexture"));
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_worm_drill_topTexture"));
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_worm_faceTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}


CWorm* CWorm::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CWorm* pMonster = new CWorm(pGraphicDev);

    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("CWorm Create Failed");
        return nullptr;
    }

    return pMonster;
}

void CWorm::Free()
{
    CMonster::Free();
}