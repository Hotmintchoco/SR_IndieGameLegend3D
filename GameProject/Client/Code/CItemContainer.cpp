#include "pch.h"
#include "CItemContainer.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CSphereCollider.h"
#include "CPlayer.h"
#include "CCollisionMgr.h"
#include "CRoomLayer.h"
#include "CWeaponPickup.h"

CItemContainer::CItemContainer(LPDIRECT3DDEVICE9 pGraphicDev, EObjectType eType)
    : CGameObject(pGraphicDev), m_eInnerItemType(eType)
{
}

CItemContainer::~CItemContainer()
{
}

HRESULT CItemContainer::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pColliderCom->Set_Owner(this);
    m_pColliderCom->Set_Extents(_vec3{ 0.5f, 1.f, 0.5f});
    m_pColliderCom->Set_DiffPos(_vec3{ 0.f, 0.5f, 0.f});
    m_pTransformCom->Set_Pos(_vec3{ 60.f, 0.f, 60.f });

    m_iFrameCount = m_pSideTexture->GetCount();

    return S_OK;
}

_int CItemContainer::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);

    CCollisionMgr::GetInstance()->Add_Collider(COLL_OBSTACLE, m_pColliderCom);

    if (m_bOnAnimation)
    {
        m_fSingleFrameAccTime += fTimeDelta;
        if (m_fSingleFrameAccTime >= m_fFrameInterval)
        {
            m_fSingleFrameAccTime -= m_fFrameInterval;
            ++m_iCurrentFrame;
            if (m_iCurrentFrame == m_iFrameCount)
            {
                m_bAnimationFinished = true;
                m_bOnAnimation = false;
            }
        }
    }

    return iExit;
}

void CItemContainer::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CItemContainer::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    
    m_pPlateTexture->Set_Texture(0);
    m_pPlateBuffer->Render_Buffer();

    if (!m_bAnimationFinished)
    {
        m_pSideTexture->Set_Texture(m_iCurrentFrame);
        m_pSideBuffer->Render_Buffer();
    }
}

HRESULT CItemContainer::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Plate Buffer
    pComponent = m_pPlateBuffer = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ItemContainer_Plate_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_PlateBuffer", pComponent });

    // Side Buffer
    pComponent = m_pSideBuffer = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ItemContainer_Side_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_SideBuffer", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Collider
    pComponent = m_pColliderCom = dynamic_cast<CBoxCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BoxCollider"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_BoxCollider", pComponent });

    // Texture
    pComponent = m_pPlateTexture = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Gray_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_PlateTexture", pComponent });

    // Texture
    pComponent = m_pSideTexture = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ItemContainer_Side_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_SideTexture", pComponent });


    return S_OK;
}

void CItemContainer::Open()
{
    m_bOnAnimation = true;

    CGameObject* pGameObject = CWeaponPickup::Create(m_pGraphicDev, this, m_eInnerItemType);
    if (!pGameObject) return;

    if (FAILED(m_pOwner->Add_GameObject(L"WeaponPickup", pGameObject)))
        return;
}

void CItemContainer::OnCollisionEnter(COLLINFO eCollInfo)
{
    auto& [pMyCol, pOtherCol, iMyID, iOtherID] = eCollInfo;

    switch (iOtherID)
    {
    case COLLISIONID::COLL_PROJECTILE:
        m_pColliderCom->Set_IsActive(false);
        Open();
        break;
    default:
        break;
    }
}

CItemContainer* CItemContainer::Create(LPDIRECT3DDEVICE9 pGraphicDev, EObjectType eType)
{
    CItemContainer* pObject = new CItemContainer(pGraphicDev, eType);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("CItemContainer Create Failed");
        return nullptr;
    }

    return pObject;
}

void CItemContainer::Free()
{
    CGameObject::Free();
}
