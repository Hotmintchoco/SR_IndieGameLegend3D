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
#include "CGameStatusMgr.h"
#include "CRoomLayer.h"
#include "Client_Struct.h"
#include "CSoundMgr.h"

_uint CMonster::iMonsterIdx=0;

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev), m_iHp(0), m_fFrame(0.f), m_fHitEffectTime(0.1f), m_fHitEffectElapsedTime(0.f), m_bHitState(false)
{
    ++iMonsterIdx;
    /* 성철 */
    if (!m_pOwner) m_pOwner = CGameStatusMgr::GetInstance()->GetCurrentRoomLayer();
    /* --- */
}


CMonster::~CMonster()
{
}

HRESULT CMonster::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pColliderCom->Set_CollisionID(COLL_MONSTER);
    
    Set_IsActive(false);
    m_pColliderCom->Set_IsActive(false);

    static_cast<CRoomLayer*>(m_pOwner)->IncreaseEntityCount();
    static_cast<CRoomLayer*>(m_pOwner)->m_OnRoomEvent.AddBinding(GetToken(), [this](const TRoomEventCtx& t) {OnRoomEvent(t); });

    __super::Ready_GameObject();
    return S_OK;
}

_int CMonster::Update_GameObject(const _float& fTimeDelta)
{
    if (!Get_IsActive()) return S_OK;
    m_fElapsedTime += fTimeDelta;

    if (!m_pColliderCom->Get_IsActive())
    {
        m_pColliderCom->Set_IsActive(true);
    }
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);
    
    Update_HitState(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);


    if (m_bDelete == true)
    {
        /* 성철 */
        if (CRoomLayer* pLayer = dynamic_cast<CRoomLayer*>(m_pOwner))
        {
            pLayer->DecreaseEntityCount();
        }
        /* ---- */
        Set_Dead(true);

        /* 성철 : 임시 몬스터 사망 소리 */
        CSoundMgr::GetInstance()->PlaySFX(L"sfxKill.wav");
        /* ------------------------- */
    }

    return iExit;
}

void CMonster::LateUpdate_GameObject(const _float& fTimeDelta)
{
    if (!Get_IsActive()) return;

    CGameObject::LateUpdate_GameObject(fTimeDelta);

    // 충돌 처리 여부를 위해 충돌 매니저에 몬스터의 콜라이더를 등록
	CCollisionMgr::GetInstance()->Add_Collider(COLL_MONSTER, m_pColliderCom);

    //_vec3 vPos;
    //m_pTransformCom->Get_Info(INFO_POS, &vPos);
    //CGameObject::Compute_ViewZ(&vPos);
}

void CMonster::Render_GameObject()
{

}

void CMonster::OnCollisionEnter(COLLINFO eCollInfo)
{
	CCollider* pCollider = eCollInfo.pOtherCollider;
    
    if (pCollider && pCollider->Get_CollisionID() == COLL_PROJECTILE)
    {
        m_bHitState = true;
        m_fHitEffectElapsedTime = 0.f;
        m_iHp -= 1; /* 성철 : Collider ID, 데미지 받는 방식 임시로 바꿔둠 */
    }
}

void CMonster::OnCollisionStay(COLLINFO eCollInfo)
{
	// CollisionWithMonster(eCollInfo);
}

void CMonster::Update_HitState(const _float& fTimeDelta)
{
    if (m_bHitState == true && m_fHitEffectElapsedTime < m_fHitEffectTime)
    {
        m_fHitEffectElapsedTime += fTimeDelta;
    }
    else  if (m_fHitEffectElapsedTime >= m_fHitEffectTime)
    {
        m_bHitState = false;
        m_fHitEffectElapsedTime = 0.f;
    }
}

void CMonster::Enable_HitRenderState()
{
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

    // 빨간색
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 0, 0));
}

void CMonster::Disable_HitRenderState()
{
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
}

void CMonster::CollisionWithMonster(COLLINFO eCollInfo)
{
    if (eCollInfo.iMyID != eCollInfo.iOtherID)
        return;

    // 나와 상대방의 위치 및 반지름 가져오기
    _vec3 vMyPos, vOtherPos;
    Get_Pos(&vMyPos);

    auto pOther = static_cast<CMonster*>(eCollInfo.pOtherCollider->Get_Owner());
    pOther->Get_Pos(&vOtherPos);

    float fMyRadius = eCollInfo.pMyCollider->Get_Radius(); // 내 콜라이더 반경
    float fOtherRadius = eCollInfo.pOtherCollider->Get_Radius();

    // 방향 벡터 및 거리 계산
    _vec3 vDir = vMyPos - vOtherPos;
    vDir.y = 0.f; // 수직 방향은 무시 (지면에서만 밀어내기)

    float fDist = D3DXVec3Length(&vDir);
    float fMinDist = fMyRadius + fOtherRadius;

    // 겹침(충돌) 처리
    if (fDist < fMinDist)
    {
        if (fDist == 0.f) // 완전히 똑같은 위치일 경우 방어 코드
        {
            vDir = _vec3(1.f, 0.f, 0.f);
            fDist = 0.001f;
        }
        D3DXVec3Normalize(&vDir, &vDir);

        // 겹친 거리 계산
        float fOverlap = fMinDist - fDist;

        // 내 위치만 반대 방향으로 밀어냄 (상대방 위치는 건드리지 않음)
        vMyPos += vDir * (fOverlap * 0.5f);

        // 내 트랜스폼에 새로운 위치 적용
        Set_Pos(vMyPos);
    }
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

void CMonster::Chase_Player(const _float& fTimeDelta)
{
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


void CMonster::OnRoomEvent(const TRoomEventCtx& t)
{
    switch (t.eType)
    {
    case ERoomEventType::ROOM_BEGIN:
        Set_IsActive(true);
        break;
    case ERoomEventType::RESET_ROOM:
        m_bDelete = true;
        break;
    default:
        break;
    }
}

void CMonster::Set_Pos(_vec3 vPos)
{
    m_pTransformCom->Set_Pos(vPos);
}
void		CMonster::Set_Pos(_float fX, _float fY, _float fZ)
{
    m_pTransformCom->Set_Pos(fX, fY, fZ);
}

void CMonster::Get_Pos(_vec3* pPos)
{
	m_pTransformCom->Get_Info(INFO_POS, pPos);
}
