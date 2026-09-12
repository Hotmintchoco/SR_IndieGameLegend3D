#include "pch.h"
#include "CMonster.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CPlayer.h"
#include "CRenderer.h"
#include "CCollider.h"
#include "CCollisionMgr.h"
#include "CTimerMgr.h"
//#include "CDInputMgr.h"
#include "CTerrain.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}


CMonster::~CMonster()
{
}

HRESULT CMonster::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pTransformCom->Set_Scale(2.f, 2.f, 2.f);
	m_pTransformCom->Set_Pos(10.f, 1.f, 10.f);
    m_pColliderCom->Set_Radius(1.f);

    __super::Ready_GameObject();

    return S_OK;
}

_int CMonster::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    Set_OnTerrain();
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    return iExit;
}

void CMonster::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    // 충돌 처리 여부를 위해 충돌 매니저에 몬스터의 콜라이더를 등록
	CCollisionMgr::GetInstance()->Add_Collider(COLL_MONSTER, m_pColliderCom);

    _vec3       vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    CGameObject::Compute_ViewZ(&vPos);

    CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
        ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));

    if (nullptr == pPlayerTransformCom)
        return;

    _vec3   vPlayerPos;
    pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);
    
    _vec3   vPlayerLook;
    pPlayerTransformCom->Get_Info(INFO_LOOK, &vPlayerLook);

    //m_pTransformCom->Chase_Target2(&vPlayerPos, &vPlayerLook, 3.f, fTimeDelta);

}

void CMonster::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture(0);
    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMonster::Add_Component()
{
    CComponent* pComponent = nullptr;

    // RcCol
    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_MonsterTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// Collider
    pComponent = m_pColliderCom = dynamic_cast<CCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SphereCollider"));
    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    // Calculator

    pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Calculator"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

    //if (FAILED(CTimerMgr::GetInstance()->Ready_Timer(L"Monster_Timer")))
    //    return FALSE;

    return S_OK;
}

void CMonster::Set_OnTerrain()
{
    _vec3   vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);

    CTerrainTex* pTerrainBufferCom = dynamic_cast<CTerrainTex*>
        (CManagement::GetInstance()->Get_Component(ID_STATIC, L"GameLogic_Layer", L"Terrain", L"Com_Buffer"));

    if (nullptr == pTerrainBufferCom)
        return;

    _float  fY = m_pCalculatorCom->Compute_HeightOnTerrain(&vPos, pTerrainBufferCom->Get_VtxPos());

    m_pTransformCom->Set_Pos(vPos.x, fY + 2.f, vPos.z);
}

CMonster* CMonster::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMonster* pMonster = new CMonster(pGraphicDev);

    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("CMonster Create Failed");
        return nullptr;
    }

    return pMonster;
}

void CMonster::Free()
{
    CGameObject::Free();
}
