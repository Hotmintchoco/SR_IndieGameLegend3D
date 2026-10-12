#include "pch.h"
#include "CPushableFrustum.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CCollisionMgr.h"
#include "Client_Enum.h"
#include "CRandomMgr.h"
#include "CPlayer.h"
#include "CPlayerMovement.h"
#include "CFloor.h"

CPushableFrustum::CPushableFrustum(LPDIRECT3DDEVICE9 pGraphicDev)
    : CFrustum(pGraphicDev)
{
}

CPushableFrustum::~CPushableFrustum()
{
}

HRESULT CPushableFrustum::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    // Note : 순서에 주의
    if (FAILED(CFrustum::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

_int CPushableFrustum::Update_GameObject(_float fTimeDelta)
{
    _int    iExit = CFrustum::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);
    CCollisionMgr::GetInstance()->Add_Collider(COLL_DYNAMICOBJECT, m_pColliderCom);

    if (m_bOnMoving)
    {
        m_fCurSpeed += m_fAccAmount * fTimeDelta;
        m_pTransformCom->Move_Pos(&m_vMoveDir, m_fCurSpeed, fTimeDelta);
    }

    return iExit;
}

void CPushableFrustum::LateUpdate_GameObject(_float fTimeDelta)
{
    CFrustum::LateUpdate_GameObject(fTimeDelta);
}

void CPushableFrustum::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture(0);

    m_pBufferCom->Render_Buffer();
}

void CPushableFrustum::OnCollisionEnter(COLLINFO eCollInfo)
{
    // switch (eCollInfo.iOtherID)
    // {
    // default:
    // {
    //     break;
    // }
    // }
}

void CPushableFrustum::OnCollisionStay(COLLINFO eCollInfo)
{
    switch (eCollInfo.iOtherID)
    {
    case COLLISIONID::COLL_PLAYER:
    {
        Obstacle_Collision(eCollInfo.pOtherCollider, m_pColliderCom);

        CPlayer* pPlayer = dynamic_cast<CPlayer*>(eCollInfo.pOtherCollider->Get_Owner());
        if (!pPlayer) break;
        
        _vec3 vPlayerPos = pPlayer->GetTransform()->Get_Info_Value(INFO_POS);
        _vec3 vPos = m_pTransformCom->Get_Info_Value(INFO_POS);

        _vec3 vDir = vPlayerPos - vPos; /* 물체 -> 플레이어 */
        EDirection eDir;
        if (fabsf(vDir.z) >= fabsf(vDir.x))
            eDir = (vDir.z >= 0.f) ? EDirection::NORTH : EDirection::SOUTH;
        else
            eDir = (vDir.x >= 0.f) ? EDirection::EAST : EDirection::WEST;

        _vec3 vNormal;
        switch (eDir)
        {
        case EDirection::NORTH:
            vNormal = _vec3{ 0.f, 0.f, 1.f };
            break;
        case EDirection::SOUTH:
            vNormal = _vec3{ 0.f, 0.f, -1.f };
            break;
        case EDirection::EAST:
            vNormal = _vec3{ 1.f, 0.f, 0.f };
            break;
        case EDirection::WEST:
            vNormal = _vec3{ -1.f, 0.f, 0.f };
            break;
        default:
            break;
        }

        _vec3 vPlayerVel = pPlayer->GetMovement()->GetVelocity();
        bool bPushCondition = D3DXVec3Dot(&vPlayerVel, &vNormal) < 0.f;

        if (bPushCondition)
        {
            StartAccelerate(-vNormal);
        }

        break;
    }
    case COLLISIONID::COLL_OBSTACLE:
    {
        CGameObject* pOther = eCollInfo.pOtherCollider->Get_Owner();
        if (dynamic_cast<CFloor*>(pOther)) break;

        CTransform* pOtherTransform = static_cast<CTransform*>(pOther->Get_Component(ID_DYNAMIC, L"Com_Transform"));
        _vec3 vOtherPos = pOtherTransform->Get_Info_Value(INFO_POS);
        _vec3 vPos = m_pTransformCom->Get_Info_Value(INFO_POS);
        _vec3 vDir = vOtherPos - vPos; /* 물체 -> 다른 장애물 */
        float fDist = D3DXVec3Length(&vDir);
        if (fDist < 1e-4) break;

        _vec3 vDirNorm = vDir / fDist;

        /* 진행 방향과 44도 이내이면 멈춤 */
        if (acosf(D3DXVec3Dot(&vDirNorm, &m_vMoveDir)) >= D3DXToRadian(44.f)) break;

        m_bOnMoving = false;
        m_fCurSpeed = 0.f;

        break;
    }
    default:
    {
        break;
    }
    }
}

vector<pair<Engine::CVIBuffer*, Engine::CTransform*>> CPushableFrustum::GetRayTestTargetInfo()
{
    return vector<pair<Engine::CVIBuffer*, Engine::CTransform*>>{{m_pBufferCom, m_pTransformCom}};
}

HRESULT CPushableFrustum::Add_Component()
{
    CComponent* pComponent = nullptr;

    // PlyTex
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_GrayFrustum_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BlueFrustum_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CPushableFrustum::Destroy()
{
    Set_Dead(true);
}

void CPushableFrustum::StartAccelerate(const _vec3& vDir)
{
    m_bOnMoving = true;
    m_vMoveDir = vDir;
}

CPushableFrustum* CPushableFrustum::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CPushableFrustum* pFrustum = new CPushableFrustum(pGraphicDev);

    if (FAILED(pFrustum->Ready_GameObject()))
    {
        Safe_Release(pFrustum);
        MSG_BOX("CPushableFrustum Create Failed");
        return nullptr;
    }

    return pFrustum;
}

void CPushableFrustum::Free()
{
    CFrustum::Free();
}
