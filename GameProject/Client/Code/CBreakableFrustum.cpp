#include "pch.h"
#include "CBreakableFrustum.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CBreakableFrustum::CBreakableFrustum(LPDIRECT3DDEVICE9 pGraphicDev)
    : CFrustum(pGraphicDev)
{
}

CBreakableFrustum::~CBreakableFrustum()
{
}

HRESULT CBreakableFrustum::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    // Note : 순서에 주의
    if (FAILED(CFrustum::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

_int CBreakableFrustum::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CFrustum::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return iExit;
}

void CBreakableFrustum::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CFrustum::LateUpdate_GameObject(fTimeDelta);
}

void CBreakableFrustum::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture(0);

    m_pBufferCom->Render_Buffer();

    m_pColliderCom->Render_DebugCube();
}

void CBreakableFrustum::OnCollisionEnter(CGameObject* pOther)
{
    Set_Dead(true);
}

HRESULT CBreakableFrustum::Add_Component()
{
    CComponent* pComponent = nullptr;

    // PlyTex
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BrownFrustum_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BrownFrustum_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CBreakableFrustum* CBreakableFrustum::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CBreakableFrustum* pFrustum = new CBreakableFrustum(pGraphicDev);

    if (FAILED(pFrustum->Ready_GameObject()))
    {
        Safe_Release(pFrustum);
        MSG_BOX("CBreakableFrustum Create Failed");
        return nullptr;
    }

    return pFrustum;
}

void CBreakableFrustum::Free()
{
    CFrustum::Free();
}
