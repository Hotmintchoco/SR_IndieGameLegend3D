#include "pch.h"
#include "CProjectile.h"
#include "CProtoMgr.h"
#include "CManagement.h"

_uint CProjectile::g_iProjectileID = 0;

CProjectile::CProjectile(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev), m_iID(++g_iProjectileID)
{
}

CProjectile::~CProjectile()
{
}

HRESULT CProjectile::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CProjectile::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CheckLifeTime(fTimeDelta);

    return iExit;
}

void CProjectile::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

HRESULT CProjectile::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    return S_OK;
}

void CProjectile::BillBoard()
{
    CTransform* pPlayerTransform = static_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));

    /* 플레이어 - 투사체 변위 벡터를 vLook으로 하는 world matrix를 만듬 */
    _vec3 vPlayerPos, vBulletPos, vDisplacement, vLook;
    pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
    m_pTransformCom->Get_Info(INFO_POS, &vBulletPos);
    vDisplacement = vBulletPos - vPlayerPos;
    D3DXVec3Normalize(&vLook, &vDisplacement);

    /* up (0, 1, 0) : right 구하기 위한 방향성만 가짐 */
    _vec3 vUp{ 0.f, 1.f, 0.f };
    _vec3 vRight;
    D3DXVec3Cross(&vRight, &vUp, &vDisplacement);
    D3DXVec3Normalize(&vRight, &vRight);
    D3DXVec3Cross(&vUp, &vLook, &vRight);

    /* 스케일이 무시되면 안되므로 직접 적용 */
    _vec3 vScale = m_pTransformCom->Get_Scale();
    _vec3 vRightS = vRight * vScale.x;
    _vec3 vUpS = vUp * vScale.y;
    _vec3 vLookS = vLook * vScale.z;

    _matrix* pMatWorld = m_pTransformCom->Get_World();

    memcpy(&pMatWorld->m[0][0], &vRightS, sizeof(_vec3));
    memcpy(&pMatWorld->m[1][0], &vUpS, sizeof(_vec3));
    memcpy(&pMatWorld->m[2][0], &vLookS, sizeof(_vec3));
}

void CProjectile::CheckLifeTime(const Engine::_float& fTimeDelta)
{
    m_fLeftLifeTime -= fTimeDelta;
    if (m_fLeftLifeTime <= 0.f)
    {
        Set_Dead(true);
    }
}

void CProjectile::Free()
{
    CGameObject::Free();
}