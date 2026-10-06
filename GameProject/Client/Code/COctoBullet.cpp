#include "pch.h"
#include "COctoBullet.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CCollisionMgr.h"
#include "Client_Enum.h"
#include "CManagement.h"
#include "CEffect.h"
#include "CPlayer.h"

COctoBullet::COctoBullet(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir)
    : CProjectile(pGraphicDev)
{
    m_vStart = vStart;
    m_vDir = vDir;
}

COctoBullet::~COctoBullet()
{
}

HRESULT COctoBullet::Ready_GameObject()
{
    if (FAILED(CProjectile::Ready_GameObject()))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pData = &s_tData;

    m_pTransformCom->Set_Pos(m_vStart);
    m_pTransformCom->Set_Scale(_vec3{ 0.15f, 0.15f, 0.15f });
    m_pColliderCom->Set_Owner(this);
    m_pColliderCom->Set_Radius(0.15f);
    m_iTotalFrameCount = m_pTextureCom->GetCount();

    s_tData.fLifeTime = 100.f;
    s_tData.fSpeed = 8.f;

    return S_OK;
}

_int COctoBullet::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CProjectile::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);
    CCollisionMgr::GetInstance()->Add_Collider(COLL_MBULLET, m_pColliderCom);

    m_pTransformCom->Move_Pos(&m_vDir, s_tData.fSpeed, fTimeDelta);

    Animation(fTimeDelta);

    return iExit;
}

void COctoBullet::LateUpdate_GameObject(_float fTimeDelta)
{
    CProjectile::LateUpdate_GameObject(fTimeDelta);

    BillBoard();
}

void COctoBullet::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture(m_iCurrentTexureIdx);

    m_pBufferCom->Render_Buffer();
}

void COctoBullet::OnCollisionEnter(COLLINFO eCollInfo)
{
    auto& [pMyCol, pOtherCol, iMyID, iOtherID] = eCollInfo;

    switch (iOtherID)
    {
    case COLLISIONID::COLL_OBSTACLE:
		m_pColliderCom->Set_IsActive(false);
		Set_Dead(true);
		break;
    case COLLISIONID::COLL_PLAYER:
        static_cast<CPlayer*>(pOtherCol->Get_Owner())->OnHit(nullptr);
        m_pColliderCom->Set_IsActive(false);
        Set_Dead(true);
        break;
    default:
        break;
    }
}

HRESULT COctoBullet::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Mesh
    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_fireballTexture"));

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

void COctoBullet::Animation(const _float& fTimeDelta)
{
    m_fSingleFrameAccTime += fTimeDelta;
    if (m_fSingleFrameAccTime >= m_fFrameInterval)
    {
        m_fSingleFrameAccTime -= m_fFrameInterval;
        m_iCurrentTexureIdx = (m_iCurrentTexureIdx + 1) % m_iTotalFrameCount;
    }
}

COctoBullet* COctoBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir)
{
    COctoBullet* pBullet = new COctoBullet(pGraphicDev, vStart, vDir);

    if (FAILED(pBullet->Ready_GameObject()))
    {
        Safe_Release(pBullet);
        MSG_BOX("COctoBullet Create Failed");
        return nullptr;
    }

    return pBullet;
}

void COctoBullet::Free()
{
    CProjectile::Free();
}