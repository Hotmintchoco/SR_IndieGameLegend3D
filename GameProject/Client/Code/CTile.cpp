#include "pch.h"
#include "CTile.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include <ctime>

CTile::CTile(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CTile::CTile(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx, int iTextureIdx)
    : CGameObject(pGraphicDev), m_iPositionIdx(iPositionIdx), m_iTextureIdx(iTextureIdx)
{
}

CTile::~CTile()
{
}

HRESULT CTile::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    _vec3 vInitOffset{-6.5f, 0.f, 5.5f};
    m_pTransformCom->Set_Pos(vInitOffset.x, vInitOffset.y, vInitOffset.z);

    return S_OK;
}

_int CTile::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_PRIORITY, this);

    return iExit;
}

void CTile::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CTile::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture(m_iTextureIdx);

    m_pBufferCom->Render_Buffer();
}

HRESULT CTile::Add_Component()
{
    CComponent* pComponent = nullptr;

    // TerrainTex
    pComponent = m_pBufferCom = dynamic_cast<CPlaneTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_PlaneTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Tile_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });


    return S_OK;
}

CTile* CTile::Create(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx, int iTextureIdx)
{
    CTile* pTile = new CTile(pGraphicDev, iPositionIdx, iTextureIdx);

    if (FAILED(pTile->Ready_GameObject()))
    {
        Safe_Release(pTile);
        MSG_BOX("CTile Create Failed");
        return nullptr;
    }

    return pTile;
}

void CTile::Free()
{
    CGameObject::Free();
}
