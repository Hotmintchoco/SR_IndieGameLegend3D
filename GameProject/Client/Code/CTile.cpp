#include "pch.h"
#include "CTile.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CRoomLoadingMgr.h"

CTile::CTile(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CTile::CTile(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx)
    : CGameObject(pGraphicDev), m_iPositionIdx(iPositionIdx)
{
    _vec3 vSize = CRoomLoadingMgr::GetInstance()->GetInnerRoomSize();
    const int iSizeX = (int)vSize.x;

    m_tPositionIdx2D = TTileIdx{ m_iPositionIdx / iSizeX, m_iPositionIdx % iSizeX };
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

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return iExit;
}

void CTile::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CTile::Render_GameObject()
{
}

HRESULT CTile::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });


    return S_OK;
}

void CTile::Free()
{
    CGameObject::Free();
}
