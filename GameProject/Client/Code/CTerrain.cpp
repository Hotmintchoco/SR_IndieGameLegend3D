#include "pch.h"
#include "CTerrain.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}


CTerrain::~CTerrain()
{
}

HRESULT CTerrain::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;


    return S_OK;
}

_int CTerrain::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGrop(RENDER_PRIORITY, this);


    return iExit;
}

void CTerrain::LateUpdate_GameObject(const _float& fTimeDelta)
{

    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CTerrain::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

    m_pTextureCom->Set_Texture(0);

    if (FAILED(Set_Material()))
        return;

    m_pBufferCom->Render_Buffer();
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

}

HRESULT CTerrain::Add_Component()
{
    CComponent* pComponent = nullptr;

    // TerrainTex
    pComponent = m_pBufferCom = dynamic_cast<CTerrainTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_TerrainTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_TerrainTexture2"));

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

HRESULT CTerrain::Set_Material()
{
    D3DMATERIAL9        tMtrl;
    ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

    tMtrl.Diffuse  = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
    tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
    tMtrl.Ambient  = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);

    tMtrl.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
    tMtrl.Power = 0.f;

    m_pGraphicDev->SetMaterial(&tMtrl);

    return S_OK;
}


CTerrain* CTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CTerrain* pTerrain = new CTerrain(pGraphicDev);

    if (FAILED(pTerrain->Ready_GameObject()))
    {
        Safe_Release(pTerrain);
        MSG_BOX("CTerrain Create Failed");
        return nullptr;
    }

    return pTerrain;
}

void CTerrain::Free()
{
    CGameObject::Free();
}
