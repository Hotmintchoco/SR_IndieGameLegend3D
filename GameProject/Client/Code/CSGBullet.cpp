#include "pch.h"
#include "CSGBullet.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CCollisionMgr.h"
#include "Client_Enum.h"

CSGBullet::CSGBullet(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir, const float& fScaleCoef)
    : CProjectile(pGraphicDev), m_vStart(vStart), m_vDir(vDir), m_fScaleCoef(fScaleCoef)
{
}

CSGBullet::~CSGBullet()
{
}

HRESULT CSGBullet::Ready_GameObject()
{
    if (FAILED(CProjectile::Ready_GameObject()))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pTransformCom->Set_Pos(m_vStart);
    m_pTransformCom->Set_Scale(_vec3{ m_fInitScale * m_fScaleCoef, m_fInitScale * m_fScaleCoef, m_fInitScale * m_fScaleCoef });
    m_pColliderCom->Set_Owner(this);
    m_pColliderCom->Set_Radius(0.3f);
    m_iTotalFrameCount = m_pTextureCom->GetCount();

    return S_OK;
}

_int CSGBullet::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CProjectile::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);
    CCollisionMgr::GetInstance()->Add_Collider(COLL_PROJECTILE, m_pColliderCom);

    m_pTransformCom->Move_Pos(&m_vDir, m_fSpeed, fTimeDelta);

    SyncScaleToLifeTime();

    CheckLifeTime(fTimeDelta);

    Animation(fTimeDelta);

    return iExit;
}

void CSGBullet::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CProjectile::LateUpdate_GameObject(fTimeDelta);

    BillBoard();
}

void CSGBullet::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture(m_iCurrentTexureIdx);

    m_pBufferCom->Render_Buffer();
}

void CSGBullet::OnCollisionEnter(CGameObject* pObject)
{
    Set_Dead(true);
}

HRESULT CSGBullet::Add_Component()
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
    pComponent = m_pColliderCom = dynamic_cast<CSphereCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SphereCollider"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    return S_OK;
}

void CSGBullet::Animation(const _float& fTimeDelta)
{
    m_fSingleFrameAccTime += fTimeDelta;
    if (m_fSingleFrameAccTime >= m_fFrameInterval)
    {
        m_fSingleFrameAccTime -= m_fFrameInterval;
        m_iCurrentTexureIdx = (m_iCurrentTexureIdx + 1) % m_iTotalFrameCount;
    }
}

void CSGBullet::CheckLifeTime(const _float& fTimeDelta)
{
    m_fLeftLifeTime -= fTimeDelta;
    if (m_fLeftLifeTime <= 0.f)
    {
        Set_Dead(true);
    }

    if (m_fCurrentScale <= 0.f)
    {
        Set_Dead(true);
    }
}

void CSGBullet::SyncScaleToLifeTime()
{
    float fLastTime = m_fLifeTime - m_fLeftLifeTime;
    m_fCurrentScale = m_fInitScale - fLastTime * m_fShrinkSpeed;
    if (m_fCurrentScale <= 0.f) return;

    m_pTransformCom->Set_Scale(m_fCurrentScale, m_fCurrentScale, m_fCurrentScale);
}

CSGBullet* CSGBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir, const float& fScaleCoef)
{
    CSGBullet* pBullet = new CSGBullet(pGraphicDev, vStart, vDir, fScaleCoef);

    if (FAILED(pBullet->Ready_GameObject()))
    {
        Safe_Release(pBullet);
        MSG_BOX("CSGBullet Create Failed");
        return nullptr;
    }

    return pBullet;
}

void CSGBullet::Free()
{
    CProjectile::Free();
}