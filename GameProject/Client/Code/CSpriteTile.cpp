#include "pch.h"
#include "CSpriteTile.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CRoomLoadingMgr.h"

CSpriteTile::CSpriteTile(LPDIRECT3DDEVICE9 pGraphicDev)
    : CTile(pGraphicDev)
{
    SetType(ETileType::SPRITE);
}

CSpriteTile::CSpriteTile(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx, int iTextureIdx)
    : CTile(pGraphicDev, iPositionIdx), m_iTextureIdx(iTextureIdx)
{
    SetType(ETileType::SPRITE);
}

CSpriteTile::CSpriteTile(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx, int iTextureIdx, bool bResistContamination)
    : CTile(pGraphicDev, iPositionIdx), m_iTextureIdx(iTextureIdx), m_bResistContamination(bResistContamination)
{
    SetType(ETileType::SPRITE);
}

CSpriteTile::~CSpriteTile()
{
}

HRESULT CSpriteTile::Ready_GameObject()
{
    if (FAILED(CTile::Ready_GameObject()))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CSpriteTile::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CTile::Update_GameObject(fTimeDelta);

    UpdateAnimationTile(fTimeDelta);

    return iExit;
}

void CSpriteTile::UpdateAnimationTile(const _float& fTimeDelta)
{
    if (!m_bContaminated) return;

    m_fAnimSingleFrameAccTime += fTimeDelta;

    if (m_fAnimSingleFrameAccTime > m_fAnimFrameInterval)
    {
        m_fAnimSingleFrameAccTime -= m_fAnimFrameInterval;
        m_iAnimTextureIndex = (m_iAnimTextureIndex + 1) % m_pAnimTextureCom->GetCount();
    }

    m_fContaminationLeftTime -= fTimeDelta;
    if (m_fContaminationLeftTime <= 0.f)
    {
        m_bContaminated = false;
        m_eContaminationType = EContaminateType::NONE;
        m_iAnimTextureIndex = 0;
        m_fAnimSingleFrameAccTime = 0.f;
    }
}

void CSpriteTile::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CTile::LateUpdate_GameObject(fTimeDelta);
}

void CSpriteTile::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    if (m_bContaminated)
    {
        m_pAnimTextureCom->Set_Texture(m_iAnimTextureIndex);
    }
    else
    {
        m_pTextureCom->Set_Texture(m_iTextureIdx);
    }

    m_pBufferCom->Render_Buffer();
}

void CSpriteTile::Contaminate(EContaminateType eType, float fDuration)
{
    if (m_bResistContamination) return;

    if (m_pAnimTextureCom)
    {
        m_mapComponent[ID_STATIC].erase(L"Com_AnimTexture");
        Safe_Release(m_pAnimTextureCom);
    }

    switch (eType)
    {
    case EContaminateType::LAVA:
        m_eContaminationType = EContaminateType::LAVA;
        m_pAnimTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_TileLava_Texture"));
        break;
    default:
        break;
    }

    m_mapComponent[ID_STATIC].insert({ L"Com_AnimTexture", m_pAnimTextureCom });

    m_bContaminated = true;
    m_fContaminationLeftTime = max(m_fContaminationLeftTime, fDuration);
}

HRESULT CSpriteTile::Add_Component()
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

    return S_OK;
}

CSpriteTile* CSpriteTile::Create(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx, int iTextureIdx)
{
    CSpriteTile* pTile = new CSpriteTile(pGraphicDev, iPositionIdx, iTextureIdx);

    if (FAILED(pTile->Ready_GameObject()))
    {
        Safe_Release(pTile);
        MSG_BOX("CSpriteTile Create Failed");
        return nullptr;
    }

    return pTile;
}

CSpriteTile* CSpriteTile::Create(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx, int iTextureIdx, bool bResistContamination)
{
    CSpriteTile* pTile = new CSpriteTile(pGraphicDev, iPositionIdx, iTextureIdx, bResistContamination);

    if (FAILED(pTile->Ready_GameObject()))
    {
        Safe_Release(pTile);
        MSG_BOX("CSpriteTile Create Failed");
        return nullptr;
    }

    return pTile;
}

void CSpriteTile::Free()
{
    CTile::Free();
}
