#include "pch.h"
#include "CUnbreakableFrustum.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CUnbreakableFrustum::CUnbreakableFrustum(LPDIRECT3DDEVICE9 pGraphicDev)
    : CFrustum(pGraphicDev)
{
}

CUnbreakableFrustum::~CUnbreakableFrustum()
{
}

HRESULT CUnbreakableFrustum::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    // Note : 순서에 주의
    if (FAILED(CFrustum::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

_int CUnbreakableFrustum::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CFrustum::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return iExit;
}

void CUnbreakableFrustum::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CFrustum::LateUpdate_GameObject(fTimeDelta);
}

void CUnbreakableFrustum::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture(0);

    m_pBufferCom->Render_Buffer();
}

void CUnbreakableFrustum::OnCollisionEnter(CGameObject* pOther)
{

}

HRESULT CUnbreakableFrustum::Add_Component()
{
    CComponent* pComponent = nullptr;

    // PlyTex
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_GrayFrustum_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_GrayFrustum_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CUnbreakableFrustum* CUnbreakableFrustum::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CUnbreakableFrustum* pFrustum = new CUnbreakableFrustum(pGraphicDev);

    if (FAILED(pFrustum->Ready_GameObject()))
    {
        Safe_Release(pFrustum);
        MSG_BOX("CUnbreakableFrustum Create Failed");
        return nullptr;
    }

    return pFrustum;
}

void CUnbreakableFrustum::Free()
{
    CFrustum::Free();
}
