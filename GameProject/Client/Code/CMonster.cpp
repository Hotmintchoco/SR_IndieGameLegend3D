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

_uint CMonster::iMonsterIdx=0;

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev), m_iHp(0), m_fFrame(0.f), m_fHitEffectDuration(0.1f), m_fHitEffectTime(0.f), m_bHitState(false)
{
    ++iMonsterIdx;
}


CMonster::~CMonster()
{
}

HRESULT CMonster::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pColliderCom->Set_CollisionID(COLL_MONSTER);

    m_iHp = 5;

    __super::Ready_GameObject();
    return S_OK;
}

_int CMonster::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);
    
    if (m_fHitEffectTime < m_fHitEffectDuration && m_bHitState == true)
    {
        m_fHitEffectTime += fTimeDelta;
    }
    else  if (m_fHitEffectTime >= m_fHitEffectDuration)
    {
        m_bHitState = false;
    }
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    if (m_iHp <= 0)
    {
        Set_Dead(true);
    }

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
}

void CMonster::Render_GameObject()
{

}

void CMonster::OnCollisionEnter(CGameObject* pOther)
{
	//CCollider* pCollider = dynamic_cast<CCollider*>(pOther->Get_Component(ID_DYNAMIC, L"Com_Collider"));
    //
    //if (pCollider && pCollider->Get_CollisionID() == COLL_PBULLET)
    //{
    //    m_fHitEffectTime = 0.f;
    //    m_bHitState = true;
	//}
    /////////////////////테스트 가나다라마바사 ㅇ마ㅣㅓ랴ㅐㅜㅐㅣㅜ쿠ㅏㅣㄹㅇ널ㄹ라ㅣㄴ얼
    int a = 0;
    /////////////////////테스트 가나다라마바사 ㅇ마ㅣㅓ랴ㅐㅜㅐㅣㅜ쿠ㅏㅣㄹㅇ널ㄹ라ㅣㄴ얼
    int b = 1;
    m_fHitEffectTime = 0.f;
    m_bHitState = true;
}

void CMonster::Enable_HitRenderState()
{

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

    // 빨간색
    m_pGraphicDev->SetRenderState(
        D3DRS_TEXTUREFACTOR,
        D3DCOLOR_ARGB(255, 255, 0, 0)
    );
}

void CMonster::Disable_HitRenderState()
{

    m_pGraphicDev->SetTextureStageState(
        0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

    m_pGraphicDev->SetTextureStageState(
        0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

}

HRESULT CMonster::Add_Component()
{
    CComponent* pComponent = nullptr;

    // RcCol
    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

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

    m_pTransformCom->Set_Pos(vPos.x, fY + m_pTransformCom->m_vScale.y, vPos.z);
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


void CMonster::Set_Pos(_vec3 vPos)
{
    m_pTransformCom->Set_Pos(vPos);
}
void		CMonster::Set_Pos(_float fX, _float fY, _float fZ)
{
    m_pTransformCom->Set_Pos(fX, fY, fZ);
}
