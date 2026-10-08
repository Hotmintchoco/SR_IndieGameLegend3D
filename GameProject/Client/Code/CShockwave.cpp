#include "pch.h"
#include "CShockwave.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include <ctime>

CShockwave::CShockwave(LPDIRECT3DDEVICE9 pGraphicDev)
    : CParticle(pGraphicDev)
{
}


CShockwave::~CShockwave()
{
}

HRESULT CShockwave::Ready_GameObject()
{
    CParticle::Ready_GameObject();
    if (FAILED(Add_Component()))
        return E_FAIL;


    return S_OK;
}

_int CShockwave::Update_GameObject(_float fTimeDelta)
{
    _int    iExit = CParticle::Update_GameObject(fTimeDelta);

    m_fFrame += 6.f * fTimeDelta;

    if (6.f <= m_fFrame)
    {
        m_fFrame = 5.5f;
        Set_Dead(true);
    }
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);

    return iExit;
}

void CShockwave::LateUpdate_GameObject(_float fTimeDelta)
{
    CParticle::LateUpdate_GameObject(fTimeDelta);

    LookAtPlayer();
}

void CShockwave::Render_GameObject()
{
    CParticle::Render_GameObject();

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture((_uint)m_fFrame);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CShockwave::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_shockwave_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // RcTex
    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });


    return S_OK;
}


CShockwave* CShockwave::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CShockwave* pShockwave = new CShockwave(pGraphicDev);

    if (FAILED(pShockwave->Ready_GameObject()))
    {
        Safe_Release(pShockwave);
        MSG_BOX("CShockwave Create Failed");
        return nullptr;
    }

    return pShockwave;
}


CShockwave* CShockwave::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale)
{
    CShockwave* pShockwave = new CShockwave(pGraphicDev);

    if (FAILED(pShockwave->Ready_GameObject()))
    {
        Safe_Release(pShockwave);
        MSG_BOX("CShockwave Create Failed");
        return nullptr;
    }
    pShockwave->Set_Pos(vPos);
    pShockwave->Set_Scale(vScale);
    return pShockwave;
}


void CShockwave::Free()
{
    CParticle::Free();
}
