#include "pch.h"
#include "CButtonTile.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CRoomLoadingMgr.h"
#include "CCollisionMgr.h"
#include "CRoomLayer.h"

CButtonTile::CButtonTile(LPDIRECT3DDEVICE9 pGraphicDev)
    : CTile(pGraphicDev)
{
    SetType(ETileType::BUTTON);
}

CButtonTile::CButtonTile(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx, bool bFixed)
    : CTile(pGraphicDev, iPositionIdx), m_bFixed(bFixed)
{
    SetType(ETileType::BUTTON);
}

CButtonTile::~CButtonTile()
{
}

HRESULT CButtonTile::Ready_GameObject()
{
    if (FAILED(CTile::Ready_GameObject()))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pColliderCom->Set_CollisionID(COLL_BUTTON);
    m_pColliderCom->Set_Extents(_vec3{ 0.3f, 0.3f, 0.3f });
    m_pColliderCom->Set_DiffPos(_vec3{ 0.f, 0.4f, 0.f });

    static_cast<CRoomLayer*>(m_pOwner)->OnButtonInteracted(false);

    return S_OK;
}

_int CButtonTile::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CTile::Update_GameObject(fTimeDelta);

    CCollisionMgr::GetInstance()->Add_Collider(COLL_BUTTON, m_pColliderCom);

    return iExit;
}

void CButtonTile::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CTile::LateUpdate_GameObject(fTimeDelta);
}

void CButtonTile::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    if (m_bPressed)
    {
        m_pDownTextureCom->Set_Texture(0);
        m_pDownBufferCom->Render_Buffer();
    }
    else
    {
        m_pUpTextureCom->Set_Texture(0);
        m_pUpBufferCom->Render_Buffer();
    }

}

void CButtonTile::OnCollisionEnter(CGameObject* pOther)
{
    if (pOther == this) return;

    ++m_iCollisionCount;

    static_cast<CRoomLayer*>(m_pOwner)->OnButtonInteracted(true);

    m_bPressed = true;
}

void CButtonTile::OnCollisionExit(CGameObject* pOther)
{
    if (pOther == this) return;

    --m_iCollisionCount;

    if (m_bFixed) return;

    if (m_iCollisionCount == 0)
    {
        static_cast<CRoomLayer*>(m_pOwner)->OnButtonInteracted(false);
        m_bPressed = false;
    }
}

HRESULT CButtonTile::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Vertex
    pComponent = m_pUpBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Button_Up_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_UpBuffer", pComponent });
    
    // Vertex
    pComponent = m_pDownBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Button_Down_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_DownBuffer", pComponent });

    // Texture
    pComponent = m_pUpTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Button_Up_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_UpTexture", pComponent });

    // Texture
    pComponent = m_pDownTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Button_Down_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_DownTexture", pComponent });

    // Collider
    pComponent = m_pColliderCom = dynamic_cast<CBoxCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BoxCollider"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_pColliderCom->Set_Owner(this);

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    return S_OK;
}

CButtonTile* CButtonTile::Create(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx, bool bFixed)
{
    CButtonTile* pTile = new CButtonTile(pGraphicDev, iPositionIdx, bFixed);

    if (FAILED(pTile->Ready_GameObject()))
    {
        Safe_Release(pTile);
        MSG_BOX("CButtonTile Create Failed");
        return nullptr;
    }

    return pTile;
}

void CButtonTile::Free()
{
    CTile::Free();
}
