#include "pch.h"
#include "CArrow.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CCollisionMgr.h"
#include "Client_Enum.h"
#include "CCrossBuffer.h"
#include "CRayCaster.h"
#include "CManagement.h"
#include "CStage.h"
#include "IRayTestable.h"
#include "CRoomLayer.h"
#include "CTimerMgr.h"

CArrow::CArrow(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir, float fShotPower)
    : CProjectile(pGraphicDev), m_vStart(vStart), m_vDir(vDir), m_fSpeed(fShotPower * m_tData.fMaxSpeed)
{
}

CArrow::CArrow(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir, float fShotPower, const TArrowData& t)
    : CProjectile(pGraphicDev), m_vStart(vStart), m_vDir(vDir), m_tData(t)
{
    m_fSpeed = fShotPower * t.fMaxSpeed;
}

CArrow::~CArrow()
{   
}

HRESULT CArrow::Ready_GameObject()
{
    if (FAILED(CProjectile::Ready_GameObject()))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pData = &m_tData;

    InitTransform();
    
    _vec3 vLook;
    m_pTransformCom->Get_Info(INFO_LOOK, &vLook);
    D3DXVec3Normalize(&vLook, &vLook);
    m_vVelocity = vLook * m_fSpeed;

    m_pColliderCom->Set_Owner(this);
    m_pColliderCom->Set_Radius(0.1f);

    return S_OK;
}

void CArrow::InitTransform()
{
    m_pTransformCom->Set_Pos(m_vStart);
    m_pTransformCom->Set_Scale(m_tData.vInitScale);

    _vec3 vLook, vUp, vRight;
    D3DXVec3Normalize(&vLook, &m_vDir);
    _vec3 vWorldUp = _vec3{ 0.f, 1.f, 0.f };
    D3DXVec3Cross(&vRight, &vWorldUp, &vLook);
    D3DXVec3Normalize(&vRight, &vRight);
    D3DXVec3Cross(&vUp, &vLook, &vRight);

    _matrix* pWorld = m_pTransformCom->Get_World();
    _vec3 vScale = m_pTransformCom->Get_Scale();
    vRight = vScale.x * vRight;
    vUp = vScale.y * vUp;
    vLook = vScale.z * vLook;

    memcpy(&pWorld->m[0][0], &vRight, sizeof(_vec3));
    memcpy(&pWorld->m[1][0], &vUp, sizeof(_vec3));
    memcpy(&pWorld->m[2][0], &vLook, sizeof(_vec3));

    m_pTransformCom->WorldMatrixDecompose();
}

void CArrow::ExertGravity(const float fTimeDelta)
{
    m_vVelocity.y -= m_tData.fGravityCoef * fTimeDelta;
    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    vPos += m_vVelocity * fTimeDelta;
    m_pTransformCom->Set_Pos(vPos);
}

void CArrow::SyncTransformToVelocity()
{
    _vec3 vLook, vUp, vRight;
    D3DXVec3Normalize(&vLook, &m_vVelocity);
    _vec3 vWorldUp = _vec3{ 0.f, 1.f, 0.f };
    D3DXVec3Cross(&vRight, &vWorldUp, &vLook);
    D3DXVec3Normalize(&vRight, &vRight);
    D3DXVec3Cross(&vUp, &vLook, &vRight);

    _matrix* pWorld = m_pTransformCom->Get_World();
    _vec3 vScale = m_tData.vInitScale;
    vRight = vScale.x * vRight;
    vUp = vScale.y * vUp;
    vLook = vScale.z * vLook;

    memcpy(&pWorld->m[0][0], &vRight, sizeof(_vec3));
    memcpy(&pWorld->m[1][0], &vUp, sizeof(_vec3));
    memcpy(&pWorld->m[2][0], &vLook, sizeof(_vec3));

    m_pTransformCom->WorldMatrixDecompose();
}

_int CArrow::Update_GameObject(_float fTimeDelta)
{
    m_vPrevPos = m_pTransformCom->Get_Info_Value(INFO_POS);

    _int iExit = CProjectile::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);
    CCollisionMgr::GetInstance()->Add_Collider(COLL_PROJECTILE, m_pColliderCom);

    if (!m_bStopped)
    {
        ExertGravity(fTimeDelta);
        SyncTransformToVelocity();
    }

    for (auto pObj : m_vecRayTestTarget)
    {
        if (PreciseHitTest(pObj))
            break;
    }

    if (m_pTransformCom->Get_Info_Value(INFO_POS).y < -10.f) Set_Dead(true);

    return iExit;
}

bool CArrow::PreciseHitTest(CGameObject* pTarget)
{
    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    _vec3 vDir = vPos - m_vPrevPos;
    float fLen = D3DXVec3Length(&vDir);
    if (fLen < 1e-6f) return false;
    vDir /= fLen;

    CRayCaster* pRayCaster = static_cast<CRayCaster*>(CManagement::GetInstance()->Get_GameObject(L"GameLogic_Layer", L"RayCaster"));
    THitInfo t{};

    if (IRayTestable* pRayTestable = dynamic_cast<IRayTestable*>(pTarget))
    {
        vector<pair<CVIBuffer*, CTransform*>> vecInfo = pRayTestable->GetRayTestTargetInfo();
        for (auto& [pBuffer, pTransform] : vecInfo)
        {
            pRayCaster->RayTest(t, m_vPrevPos, vDir, pBuffer, pTransform->Get_World());
        }
    }

    if (t.bHit && t.fDist < fLen)
    {
        m_pTransformCom->Set_Pos(t.fHitPoint);
        m_bStopped = true;
        return true;
    }

    return false;
}

void CArrow::LateUpdate_GameObject(_float fTimeDelta)
{
    CProjectile::LateUpdate_GameObject(fTimeDelta);
}

void CArrow::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture(0);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CArrow::OnCollisionEnter(COLLINFO eCollInfo)
{
    auto& [pMyCol, pOtherCol, iMyID, iOtherID] = eCollInfo;

    switch (eCollInfo.iOtherID)
    {
    case COLLISIONID::COLL_OBSTACLE:
        if (IRayTestable* pRayTestable = dynamic_cast<IRayTestable*>(pOtherCol->Get_Owner()))
        {
            m_vecRayTestTarget.push_back(pOtherCol->Get_Owner());
            PreciseHitTest(pOtherCol->Get_Owner());
        }
        break;
    case COLLISIONID::COLL_MONSTER:
        Set_Dead(true);
        break;
    default:
        break;
    }
}

void CArrow::OnCollisionExit(COLLINFO eCollInfo)
{
}

HRESULT CArrow::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Mesh
    pComponent = m_pBufferCom = dynamic_cast<CCrossBuffer*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Cross_Buffer"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Arrow_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // Collider
    pComponent = m_pColliderCom = dynamic_cast<CSphereCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SphereCollider"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    return S_OK;
}

CArrow* CArrow::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir, float fShotPower)
{
    CArrow* pBullet = new CArrow(pGraphicDev, vStart, vDir, fShotPower);

    if (FAILED(pBullet->Ready_GameObject()))
    {
        Safe_Release(pBullet);
        MSG_BOX("CArrow Create Failed");
        return nullptr;
    }

    return pBullet;
}

CArrow* CArrow::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir, float fShotPower, const TArrowData& t)
{
    CArrow* pBullet = new CArrow(pGraphicDev, vStart, vDir, fShotPower, t);

    if (FAILED(pBullet->Ready_GameObject()))
    {
        Safe_Release(pBullet);
        MSG_BOX("CArrow Create Failed");
        return nullptr;
    }

    return pBullet;
}

void CArrow::Free()
{
    CProjectile::Free();
}