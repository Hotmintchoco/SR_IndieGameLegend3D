#include "pch.h"
#include "CMiniGameBG.h"
#include "CProtoMgr.h"
#include "CManagement.h"

const _float CMiniGameBG::TILE_SIZE = 6.f;
const _float CMiniGameBG::SCROLL_SPEED = 30.f;

CMiniGameBG::CMiniGameBG(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CMiniGameBG::~CMiniGameBG()
{
}

HRESULT CMiniGameBG::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pTransformCom->Set_Scale(TILE_SIZE * 0.5f, TILE_SIZE * 0.5f, 1.f);
    m_pTransformCom->Set_Rotation_Raw({ 90.f, 0.f, 0.f });

    return S_OK;
}

HRESULT CMiniGameBG::Ready_GameObject(pair<_int, _int> pIdx)
{
    if (FAILED(Ready_GameObject()))
        return E_FAIL;

    m_vPos = { pIdx.first * TILE_SIZE, -4.f, pIdx.second * TILE_SIZE };
    m_pTransformCom->Set_Pos(m_vPos);

    return S_OK;
}

_int CMiniGameBG::Update_GameObject(_float fTimeDelta)
{
    m_vPos.z -= SCROLL_SPEED * fTimeDelta;

    _float fMinZ = (TILE_MIN_Z - 1) * TILE_SIZE;
    if (m_vPos.z < fMinZ)
        m_vPos.z += (TILE_MAX_Z - TILE_MIN_Z + 1) * TILE_SIZE;

    m_pTransformCom->Set_Pos(m_vPos);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return 0;
}

void CMiniGameBG::LateUpdate_GameObject(_float fTimeDelta)
{
}

void CMiniGameBG::Render_GameObject()
{
    DWORD dwCull, dwLighting, dwFog;
    m_pGraphicDev->GetRenderState(D3DRS_CULLMODE, &dwCull);
    m_pGraphicDev->GetRenderState(D3DRS_LIGHTING, &dwLighting);
    m_pGraphicDev->GetRenderState(D3DRS_FOGENABLE, &dwFog);

    _float fFogStart = 45.f;
    _float fFogEnd = 110.f;
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_FOGENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
    m_pGraphicDev->SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_ARGB(255, 16, 24, 48));
    m_pGraphicDev->SetRenderState(D3DRS_FOGSTART, *reinterpret_cast<DWORD*>(&fFogStart));
    m_pGraphicDev->SetRenderState(D3DRS_FOGEND, *reinterpret_cast<DWORD*>(&fFogEnd));

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Set_Texture(50);
    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_FOGENABLE, dwFog);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, dwLighting);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, dwCull);
}

HRESULT CMiniGameBG::Add_Component()
{
    CComponent* pComponent = nullptr;

    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Tile_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CMiniGameBG* CMiniGameBG::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMiniGameBG* pMiniGameBG = new CMiniGameBG(pGraphicDev);

    if (FAILED(pMiniGameBG->Ready_GameObject()))
    {
        Safe_Release(pMiniGameBG);
        MSG_BOX("CMiniGameBG Create Failed");
        return nullptr;
    }

    return pMiniGameBG;
}

CMiniGameBG* CMiniGameBG::Create(LPDIRECT3DDEVICE9 pGraphicDev, pair<_int, _int> pIdx)
{
    CMiniGameBG* pMiniGameBG = new CMiniGameBG(pGraphicDev);

    if (FAILED(pMiniGameBG->Ready_GameObject(pIdx)))
    {
        Safe_Release(pMiniGameBG);
        MSG_BOX("CMiniGameBG Create Failed");
        return nullptr;
    }

    return pMiniGameBG;
}

void CMiniGameBG::Free()
{
    CGameObject::Free();
}
