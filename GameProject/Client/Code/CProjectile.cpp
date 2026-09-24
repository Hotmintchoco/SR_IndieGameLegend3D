#include "pch.h"
#include "CProjectile.h"
#include "CRenderer.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CCollisionMgr.h"
#include "Client_Enum.h"

_uint CProjectile::g_iProjectileID = 0;

CProjectile::CProjectile(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir)
    : CGameObject(pGraphicDev), m_vStart(vStart), m_vDir(vDir), m_iID(++g_iProjectileID)
{
}

CProjectile::~CProjectile()
{
}

HRESULT CProjectile::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pTransformCom->Set_Pos(m_vStart);
    m_pTransformCom->Set_Scale(_vec3{ 0.1f, 0.1f, 0.1f });
    m_pColliderCom->Set_Owner(this);
    m_pColliderCom->Set_Radius(0.3f);
    m_iTotalFrameCount = m_pTextureCom->GetCount();

    return S_OK;
}

_int CProjectile::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);
    CCollisionMgr::GetInstance()->Add_Collider(COLL_PROJECTILE, m_pColliderCom);

    m_pTransformCom->Move_Pos(&m_vDir, m_fSpeed, fTimeDelta);

    Animation(fTimeDelta);

    return iExit;
}

void CProjectile::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    BillBoard();
}

void CProjectile::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture(m_iCurrentTexureIdx);
    
    m_pBufferCom->Render_Buffer();
}

void CProjectile::OnCollisionEnter(CGameObject* pObject)
{
    Set_Dead(true);
}

HRESULT CProjectile::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Mesh
    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Bullet_Default_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Transform
    pComponent = m_pColliderCom = dynamic_cast<CSphereCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SphereCollider"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    return S_OK;
}

void CProjectile::Animation(const _float& fTimeDelta)
{
    m_fSingleFrameAccTime += fTimeDelta;
    if (m_fSingleFrameAccTime >= m_fFrameInterval)
    {
        m_fSingleFrameAccTime -= m_fFrameInterval;
        m_iCurrentTexureIdx = (m_iCurrentTexureIdx + 1) % m_iTotalFrameCount;
    }
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

CProjectile* CProjectile::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir)
{
    CProjectile* pGun = new CProjectile(pGraphicDev, vStart, vDir);

    if (FAILED(pGun->Ready_GameObject()))
    {
        Safe_Release(pGun);
        MSG_BOX("CProjectile Create Failed");
        return nullptr;
    }

    return pGun;
}

void CProjectile::Free()
{
    CGameObject::Free();
}